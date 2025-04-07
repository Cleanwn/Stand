#include "RemoteGamer.hpp"

#include <vector>

#include <soup/ObfusString.hpp>

#include "Auth.hpp"
#include "Exceptional.hpp"
#include "FiberPool.hpp"
#include "fmt_arg.hpp"
#include "get_current_time_millis.hpp"
#include "gsType.hpp"
#include "Hooking.hpp"
#include "is_rid.hpp"
#include "is_session.hpp"
#include "JoinUtil.hpp"
#include "lang.hpp"
#include "natives.hpp"
#include "netStatus.hpp"
#include "Spinlock.hpp"
#include "pointers.hpp"
#include "rlGamerHandle.hpp"
#include "rlGamerInfo.hpp"
#include "SessionSpoofing.hpp"
#include "ScAccount.hpp"
#include "sc_presence_attributes.hpp"
#include "Script.hpp"
#include "tbLoadingSessionDetails.hpp"
#include "ThreadContext.hpp"
#include "Util.hpp"

#define DEBUG_GS_TYPE false
#define DEBUG_SELF false

namespace Stand
{
	struct RemoteGamerRequestData
	{
		RemoteGamer* const g;
		rage::netStatus status;
		rage::rlScPresenceAttribute attrs[3];

		[[nodiscard]] rage::rlScPresenceAttribute& peeraddr()
		{
			return attrs[0];
		}

		[[nodiscard]] rage::rlScPresenceAttribute& gsinfo()
		{
			return attrs[1];
		}

		[[nodiscard]] rage::rlScPresenceAttribute& gstype()
		{
			return attrs[2];
		}

		explicit RemoteGamerRequestData(RemoteGamer* const g)
			: g(g)
		{
			strcpy(peeraddr().name, soup::ObfusString("peeraddr"));
			peeraddr().int_value = 0;
			peeraddr().type = RLSC_PRESTYPE_STRING;

			strcpy(gsinfo().name, soup::ObfusString("gsinfo"));
			gsinfo().int_value = 0;
			gsinfo().type = RLSC_PRESTYPE_STRING;

			strcpy(gstype().name, soup::ObfusString("gstype"));
			gstype().int_value = 0;
			gstype().type = RLSC_PRESTYPE_S64;
		}
	};

	static Spinlock active_requests_mtx;
	static std::vector<soup::UniquePtr<RemoteGamerRequestData>> active_requests{};
	static Spinlock queued_mtx;
	static std::vector<RemoteGamer*> queued{};

	RemoteGamer* RemoteGamer::getSimple(int64_t rid)
	{
		RemoteGamer* ret = nullptr;

		for (GarbageCollected* node = RemoteGamer::gc.head.load(); node; )
		{
			auto next = node->next.load();
			if (static_cast<RemoteGamer*>(node)->rid == rid)
			{
				ret = static_cast<RemoteGamer*>(node);
				break;
			}
			node = next;
		}

		return ret;
	}

	RemoteGamer* RemoteGamer::get(int64_t rid, bool can_create, RemoteGamerUpdateType update_type)
	{
		RemoteGamer* g = getSimple(rid);
		if (g == nullptr)
		{
			if (can_create)
			{
				return create(rid, update_type == RGU_DETAILED);
			}
		}
		else
		{
			if (update_type != RGU_NONE)
			{
				g->considerRefresh(update_type == RGU_DETAILED);
			}
		}
		return g;
	}

	RemoteGamer* RemoteGamer::create(int64_t rid, bool detailed)
	{
		if (is_rid_valid(rid))
		{
			RemoteGamer* g = new RemoteGamer(rid);
			gc.add(g);
			g->request(detailed);
			return g;
		}
		return nullptr;
	}

	void RemoteGamer::considerRefresh(bool detailed)
	{
		if (requested_at == -1) // Queued for a bulk request?
		{
			if (detailed) // Want a detailed request?
			{
				// Remove from bulk queue and do detailed request instead
				EXCEPTIONAL_LOCK(queued_mtx)
				auto e = std::find(queued.begin(), queued.end(), this);
				SOUP_ASSERT(e != queued.end());
				queued.erase(e);
				EXCEPTIONAL_UNLOCK(queued_mtx)
				request(true);
			}
			return;
		}
		if (GET_MILLIS_SINCE(requested_at) > 15000) // No request active or more than 15 seconds since request was initiated?
		{
			time_t refresh_interval = 15000;
#if !DEBUG_SELF
			SOUP_IF_UNLIKELY (isUser())
			{
				refresh_interval = 1000;
			}
#endif
			if (GET_MILLIS_SINCE(status_as_of) > refresh_interval // Status is old enough to warrant a refresh?
				//|| (detailed && !response_from_detailed && couldBeImprovedWithDetailedRequest())
				)
			{
				request(detailed);
			}
		}
	}

	bool RemoteGamer::canBeTracked() const noexcept
	{
#ifdef STAND_DEBUG
		return true;
#else
		return !is_stand_dev_rid(rid);
#endif
	}

	void RemoteGamer::request(bool detailed)
	{
#if !DEBUG_SELF
		if (isUser())
		{
			const char* gsinfo = "";
			int64_t gstype = 0;

			if (auto gsinfo_attr = rage::rlScPresenceAttribute::findByName(soup::ObfusString("gsinfo").c_str()))
			{
				gsinfo = gsinfo_attr->string_value;
			}
			if (auto gstype_attr = rage::rlScPresenceAttribute::findByName(soup::ObfusString("gstype").c_str()))
			{
				gstype = gstype_attr->int_value;
			}

			response_from_detailed = true;
			processGamerState(true, gsinfo, gstype);
			return;
		}
#endif

		if (detailed)
		{
			requestDetailed();
		}
		else
		{
			requested_at = -1;
			EXCEPTIONAL_LOCK(queued_mtx)
			queued.emplace_back(this);
			EXCEPTIONAL_UNLOCK(queued_mtx)
		}
	}

	void RemoteGamer::requestDetailed()
	{
		requested_at = get_current_time_millis();

		auto data = soup::make_unique<RemoteGamerRequestData>(this);

		rage::rlGamerHandle gh(rid);
		pointers::rage_rlScPresence_GetAttributesForGamer(0, gh, data->attrs, COUNT(data->attrs), &data->status);

		EXCEPTIONAL_LOCK(active_requests_mtx)
		active_requests.emplace_back(std::move(data));
		EXCEPTIONAL_UNLOCK(active_requests_mtx)
	}

	void RemoteGamer::onTickUltimate()
	{
		for (const auto& g : tracked)
		{
			g->considerRefresh(false);
		}
		if (SessionSpoofing::copy_from != nullptr)
		{
			SessionSpoofing::copy_from->considerRefresh(false);
		}
	}

	void RemoteGamer::onTick() // TC_SCRIPT_NOYIELD
	{
		// Individual requests
		EXCEPTIONAL_LOCK(active_requests_mtx)
		for (auto i = active_requests.begin(); i != active_requests.end(); )
		{
			if ((*i)->status.isPending())
			{
				++i;
			}
			else
			{
				if ((*i)->status.m_StatusCode == rage::netStatus::SUCCEEDED)
				{
					(*i)->g->response_from_detailed = true;
					(*i)->g->processGamerState(
						(*i)->peeraddr().string_value,
						(*i)->gsinfo().string_value,
						(*i)->gstype().int_value
					);
					// is_set seems true for all of these, but sometimes false for "gstype"
				}
				i = active_requests.erase(i);
			}
		}
		EXCEPTIONAL_UNLOCK(active_requests_mtx)

		// Bulk requests
		EXCEPTIONAL_LOCK(queued_mtx)
		if (!queued.empty()
			&& !NETWORK::NETWORK_IS_GETTING_GAMER_STATUS()
			)
		{
			NETWORK::NETWORK_CLEAR_GET_GAMER_STATUS();

			const auto time = get_current_time_millis();

			rage::rlGamerHandle handles[MAX_BULK_REQUESTS]{};
			unsigned int i = 0;
			for (const auto& g : queued)
			{
				handles[i] = g->rid;
				g->requested_at = time;
				if (++i == MAX_BULK_REQUESTS)
				{
					break;
				}
			}
			pointers::send_session_info_request(handles, i);
			queued.erase(queued.begin(), queued.begin() + i);
		}
		EXCEPTIONAL_UNLOCK(queued_mtx)
	}

	bool RemoteGamer::isUser() const noexcept
	{
		return rid == g_hooking.getRealRid();
	}

	bool RemoteGamer::isQueuedOrPending() const noexcept
	{
		return requested_at != 0;
	}

	bool RemoteGamer::hasResponse() const noexcept
	{
		return online_status != TBD;
	}

	bool RemoteGamer::hasPeerAddr() const noexcept
	{
#if RG_PEERADDR
		return peeraddr.gamer_handle.isValid();
#else
		return has_peeraddr;
#endif
	}

	bool RemoteGamer::isOnline() const noexcept
	{
		return hasPeerAddr();
	}

	bool RemoteGamer::hasSession() const noexcept
	{
		return online_status > STORY_MODE;
	}

	void RemoteGamer::processGamerState(const char* peeraddr, std::string&& gsinfo, int64_t gstype) // TC_SCRIPT_NOYIELD
	{
#if RG_PEERADDR
		const bool
#endif
		has_peeraddr = (peeraddr[0] != '\0');
		if (has_peeraddr)
		{
#if RG_PEERADDR
			this->peeraddr.fromBase64(peeraddr);
#endif
		}
		else
		{
#if RG_PEERADDR
			this->peeraddr.reset();
#endif
		}
		return processGamerState(has_peeraddr || !gsinfo.empty(), std::move(gsinfo), gstype);
	}

	void RemoteGamer::processGamerState(bool signedInToScPresence, std::string&& gsinfo, int64_t gstype) // TC_OTHER|TC_SCRIPT_NOYIELD
	{
		status_as_of = get_current_time_millis();
		requested_at = 0;

		if (!signedInToScPresence
			|| !canBeTracked()
			)
		{
			story_since = 0;
			if (setOnlineStatus(RemoteGamer::OFFLINE))
			{
				onTrackUpdateSimple(LANG_GET("ONLSTS_TRK_O"));
			}
			return;
		}

		if (gsinfo.empty())
		{
			session_info.reset();
			if (story_since == 0)
			{
				story_since = get_current_time_millis();
			}
			if (!hasSession()
				|| GET_MILLIS_SINCE(story_since) > transition_millis
				|| (!is_session_or_transition_active(false) && isUser())
				)
			{
				if (setOnlineStatus(RemoteGamer::STORY_MODE))
				{
					onTrackUpdateSimple(LANG_GET("ONLSTS_TRK_S"));
				}
			}
			return;
		}

		OnlineStatus online_status;
#if DEBUG_GS_TYPE
		online_status = (OnlineStatus)(gstype + 3);
#else
		switch (gstype)
		{
		case GS_INVITE: online_status = ONLINE_INVITE; break;
		case GS_CLOSED_FRIEND: online_status = ONLINE_CLOSED_FRIEND; break;
		case GS_CLOSED_CREW: online_status = ONLINE_CLOSED_CREW; break;
		case GS_CREW: online_status = ONLINE_CREW; break;
		case GS_SOLO: online_status = ONLINE_SOLO; break;
		case GS_PUBLIC: online_status = ONLINE_PUBLIC; break;

		default:
			online_status = ONLINE_OTHER;
			break;
		}
#endif
		rage::rlSessionInfo session_info{ gsinfo };
		const bool is_update_and_changed = (hasResponse() && (this->online_status != online_status || this->session_info.id != session_info.id));
		this->online_status = online_status;
		this->session_info = std::move(session_info);
		this->story_since = 0;
		if (is_update_and_changed && isTracked())
		{
			onTrackUpdateSession();
		}
	}

	bool RemoteGamer::setOnlineStatus(OnlineStatus online_status) noexcept
	{
		const bool is_update = hasResponse();
		if (this->online_status != online_status)
		{
			this->online_status = online_status;
			if (is_update && isTracked())
			{
				return true;
			}
		}
		return false;
	}

	void RemoteGamer::onTrackUpdateSimple(const std::string& activity)
	{
		ScAccount::fromRID(rid)->requestCompletion([this, activity](const ScAccount* a)
		{
			onTrackUpdateNotify(LANG_FMT("TRK_T", FMT_ARG("name", a->getNameForUser()), FMT_ARG("activity", activity)));
		});
	}

	void RemoteGamer::onTrackUpdateSession()
	{
		ScAccount::fromRID(rid)->requestCompletion([this](const ScAccount* a)
		{
			const auto host_rid = session_info.host.gamer_handle.rockstar_id;
			if (host_rid == this->rid)
			{
				return onTrackUpdateSession(a, LANG_FMT("ONLSTS_TRK_H", getSessionTypeString()));
			}
			ScAccount::fromRID(host_rid)->requestNameLazy([this, a](const ScAccount& host) // TC_OTHER
			{
				auto host_name = host.getNameForUser();
				if (host_name.empty())
				{
					return onTrackUpdateSession(a, LANG_FMT("ONLSTS_TRK_C", getSessionTypeString()));
				}
				return onTrackUpdateSession(a, LANG_FMT("ONLSTS_TRK_CH", FMT_ARG("session_type", getSessionTypeString()), FMT_ARG("host", host_name)));
			});
		});
	}

	void RemoteGamer::onTrackUpdateSession(const ScAccount* a, std::string&& activity) // TC_OTHER
	{
		onTrackUpdateNotify(LANG_FMT("TRK_T", FMT_ARG("name", a->getNameForUser()), FMT_ARG("activity", std::move(activity))));
	}

	void RemoteGamer::onTrackUpdateNotify(std::string&& str) // TC_OTHER
	{
		Util::toast(std::move(str), tracking_update_toast_config.getToastFlags());
	}

	std::string RemoteGamer::getSessionTypeString() const noexcept
	{
#if DEBUG_GS_TYPE
		return std::to_string(online_status - 3);
#else
		switch (online_status)
		{
		case ONLINE_INVITE:
			return LANG_GET("ST_I");

		case ONLINE_CLOSED_FRIEND:
			return LANG_GET("ST_F2");

		case ONLINE_CLOSED_CREW:
			return LANG_GET("ST_CC");

		case ONLINE_CREW:
			return LANG_GET("ST_C");

		case ONLINE_SOLO:
			return LANG_GET("ST_S");

		case ONLINE_PUBLIC:
			return LANG_GET("ST_P");
		}
		return LANG_GET("ST_O");
#endif
	}

	Label RemoteGamer::toShortString() const
	{
		if (online_status == TBD)
		{
			return NOLABEL;
		}
		if (online_status == OFFLINE)
		{
			return LOC("SHTONLSTS_O2");
		}
		if (online_status == STORY_MODE)
		{
			return LOC("SHTONLSTS_S");
		}
		switch (online_status)
		{
		case ONLINE_INVITE:
			return LOC("SHTONLSTS_ST_I");

		case ONLINE_CLOSED_FRIEND:
			return LOC("SHTONLSTS_ST_CF");

		case ONLINE_CLOSED_CREW:
			return LOC("SHTONLSTS_ST_CC");

		case ONLINE_CREW:
			return LOC("SHTONLSTS_ST_C2");

		case ONLINE_SOLO:
			return LOC("SHTONLSTS_ST_S");

		case ONLINE_PUBLIC:
			return LOC("SHTONLSTS_ST_P");
		}
		return LOC("SHTONLSTS_ST_O");
	}

	std::string RemoteGamer::toUserString(bool can_issue_requests) const
	{
		if (online_status == TBD)
		{
			std::string str{ LANG_GET("ONLSTS") };
			str.append(": ");
			str.append(LANG_GET("GIP_P"));
			return str;
		}
		if (online_status == OFFLINE)
		{
			return LANG_GET("ONLSTS_O2");
		}
		if (online_status == STORY_MODE)
		{
			return LANG_GET("ONLSTS_S");
		}
		const auto host_rid = session_info.host.gamer_handle.rockstar_id;
		if (host_rid == this->rid)
		{
			return LANG_FMT("ONLSTS_H", getSessionTypeString());
		}
		if (host_rid == 0)
		{
			return LANG_FMT("ONLSTS_C", FMT_ARG("session_type", getSessionTypeString()));
		}
		auto host = ScAccount::fromRID(host_rid);
		if (can_issue_requests
			&& ScAccount::canRequestCompletionImmediately()
			)
		{
			host->requestCompletion();
		}
		return LANG_FMT("ONLSTS_CH", FMT_ARG("session_type", getSessionTypeString()), FMT_ARG("host", host->toString(false)));
	}

	std::string RemoteGamer::getGsInfo() const
	{
		if (hasSession())
		{
			return session_info.toBase64();
		}
		return {};
	}

	std::optional<int64_t> RemoteGamer::getGsType() const noexcept
	{
		switch (online_status)
		{
		case ONLINE_INVITE: return GS_INVITE;
		case ONLINE_CLOSED_FRIEND: return GS_CLOSED_FRIEND;
		case ONLINE_CLOSED_CREW: return GS_CLOSED_CREW;
		case ONLINE_CREW: return GS_CREW;
		case ONLINE_SOLO: return GS_SOLO;
		case ONLINE_PUBLIC: return GS_PUBLIC;

		case ONLINE_OTHER:
			return 69;
		}
		return std::nullopt;
	}

	bool RemoteGamer::haveTrackPermissions() noexcept
	{
		return g_auth.license_permissions == LICPERM_ULTIMATE;
	}

	bool RemoteGamer::isTrackedIgnorePermission() noexcept
	{
		return std::find(tracked.begin(), tracked.end(), this) != tracked.end();
	}

	bool RemoteGamer::isTracked() noexcept
	{
		return isTrackedIgnorePermission() && haveTrackPermissions();
	}

	bool RemoteGamer::isAnyTracked() noexcept
	{
		return !tracked.empty() && haveTrackPermissions();
	}

	void RemoteGamer::track() noexcept
	{
		if (!isTrackedIgnorePermission())
		{
			tracked.emplace_back(this);
		}
	}

	void RemoteGamer::untrack() noexcept
	{
		auto it = std::find(tracked.begin(), tracked.end(), this);
		if (it != tracked.end())
		{
			tracked.erase(it);
		}
	}

	bool RemoteGamer::useAsTrampoline(bool nuts)
	{
		bool success = false;
		g_tb_loading_session_details.enable();
		while (true)
		{
			this->considerRefresh(false);
			Script::current()->yield();
			if (this->hasResponse())
			{
				if (!this->hasSession())
				{
					break;
				}
				if (!this->isQueuedOrPending()
					&& this->session_info.host.gamer_handle.rockstar_id != g_player.getRockstarId()
					)
				{
					JoinUtil::join(this->session_info, nuts ? JM_NUTS : JM_NINJA);
					success = true;
					break;
				}
			}
		}
		g_tb_loading_session_details.disable();
		return success;
	}

	void RemoteGamer::deinit()
	{
		// .cpp
		active_requests.clear();
		queued.clear();

		// .hpp
		gc.deinit();
		tracked.clear();
	}

	bool RemoteGamer::canBeDeleted() noexcept
	{
		return hasResponse()
			&& GET_MILLIS_SINCE(status_as_of) > 10 * 60 * 1000
			&& !isTracked()
			&& this != SessionSpoofing::copy_from
			;
	}
}
