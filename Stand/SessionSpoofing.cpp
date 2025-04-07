#include "SessionSpoofing.hpp"

#include <soup/ObfusString.hpp>

#include "AbstractPlayer.hpp"
#include "CNetworkSession.hpp"
#include "CommandHideSession.hpp"
#include "CommandSpoofedHost.hpp"
#include "CommandSpoofHost.hpp"
#include "CommandSpoofSessionType.hpp"
#include "Gui.hpp"
#include "NetworkSessionUtils.hpp"
#include "RemoteGamer.hpp"
#include "rlPc.hpp"
#include "sc_presence_attributes.hpp"

namespace Stand
{
	rage::rlSessionInfo& SessionSpoofing::getRealSessionInfo()
	{
		return (*pointers::network_session)->session->rline_session.data.info;
	}

	uint64_t SessionSpoofing::getRealSessionId()
	{
		return getRealSessionInfo().id;
	}

	int64_t SessionSpoofing::getRealSessionType()
	{
		const auto host_flags = (*pointers::network_session)->m_nHostFlags[ST_Physical];
		if (host_flags & HF_IsPrivate)
		{
			if (host_flags & HF_SingleplayerOnly)
			{
				return GS_SOLO;
			}
			return GS_INVITE;
		}
		if (host_flags & HF_Closed)
		{
			if (host_flags & HF_ClosedFriends)
			{
				return GS_CLOSED_FRIEND;
			}
			if (host_flags & HF_ClosedCrew)
			{
				return GS_CLOSED_CREW;
			}
		}
		return GS_PUBLIC;
	}

	bool SessionSpoofing::isActive() noexcept
	{
		return copy_from != nullptr
			|| CommandHideSession::instance->value != 0
			|| CommandSpoofHost::instance->m_on
			|| CommandSpoofSessionType::instance->value != -1
		;
	}

	void SessionSpoofing::onDisable()
	{
		if (CommandHideSession::instance != nullptr && !isActive())
		{
			cleanup();
		}
	}

	void SessionSpoofing::cleanup()
	{
		auto* gsinfo = rage::rlScPresenceAttribute::findByName(soup::ObfusString("gsinfo").c_str());
		if (gsinfo != nullptr)
		{
			if (!*pointers::is_session_started)
			{
				gsinfo->setString("");
			}
			else
			{
				gsinfo->setString(getRealSessionInfo().toBase64());

				auto* gstype = rage::rlScPresenceAttribute::findByName(soup::ObfusString("gstype").c_str());
				if (gstype != nullptr)
				{
					gstype->setInt(getRealSessionType());
				}
			}
		}

		auto* peeraddr = rage::rlScPresenceAttribute::findByName(soup::ObfusString("peeraddr").c_str());
		if (peeraddr != nullptr
			&& peeraddr->string_value[0] == '\0'
			)
		{
			peeraddr->setString(og_peeraddr.c_str());
		}
	}

	void SessionSpoofing::onTick()
	{
		if (CommandHideSession::instance != nullptr && isActive())
		{
			onTickActive();
		}
	}

	void SessionSpoofing::onTickActive()
	{
		rage::rlSessionInfo spoofed_session_info{};
		if (copy_from != nullptr)
		{
			spoofed_session_info.fromGsInfo(copy_from->getGsInfo());
		}
		else if (CommandHideSession::instance->value == 0)
		{
			spoofed_session_info = getRealSessionInfo();
		}
		else if (CommandHideSession::instance->value == 2)
		{
			spoofed_session_info = random_session_info;
		}

		std::string spoofed_gsinfo{};
		if (spoofed_session_info.id != 0)
		{
			if (CommandSpoofHost::instance->m_on)
			{
				spoofed_session_info.host.gamer_handle = CommandSpoofedHost::instance->getRid();
			}
			spoofed_gsinfo = spoofed_session_info.toBase64();
		}
		int64_t spoofed_gstype;
		if (CommandSpoofSessionType::instance->value != -1)
		{
			spoofed_gstype = CommandSpoofSessionType::instance->value;
		}
		else
		{
			spoofed_gstype = getRealSessionType();
			if (copy_from != nullptr)
			{
				auto opt_gstype = copy_from->getGsType();
				if (opt_gstype.has_value())
				{
					spoofed_gstype = opt_gstype.value();
				}
			}
		}

		auto* gsinfo = rage::rlScPresenceAttribute::findByName(soup::ObfusString("gsinfo").c_str());
		if (gsinfo != nullptr)
		{
			gsinfo->setString(spoofed_gsinfo);

			auto* gstype = rage::rlScPresenceAttribute::findByName(soup::ObfusString("gstype").c_str());
			if (gstype != nullptr)
			{
				gstype->setInt(spoofed_gstype);
			}
		}
		else if (!spoofed_gsinfo.empty())
		{
			pointers::g_rlPc->presence_manager->SetStringAttribute(0, soup::ObfusString("gsinfo").c_str(), spoofed_gsinfo.c_str());
			pointers::g_rlPc->presence_manager->SetIntAttribute(0, soup::ObfusString("gstype").c_str(), spoofed_gstype);
		}

		if (CommandHideSession::instance->value == 3)
		{
			auto* peeraddr = rage::rlScPresenceAttribute::findByName(soup::ObfusString("peeraddr").c_str());
			if (peeraddr != nullptr
				&& peeraddr->string_value[0] != '\0'
				)
			{
				og_peeraddr = peeraddr->string_value;
				peeraddr->setString("");
			}
		}
	}
}
