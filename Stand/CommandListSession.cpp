#include "CommandListSession.hpp"

#include "CommandDivider.hpp"
#include "CommandLambdaActionScript.hpp"
#include "Exceptional.hpp"
#include "fmt_arg.hpp"
#include "gsType.hpp"
#include "Gui.hpp"
#include "Hooking.hpp"
#include "is_session.hpp"
#include "natives.hpp"
#include "NetworkSessionMessages.hpp"
#include "pointers.hpp"
#include "RageConnector.hpp"
#include "rlGamerInfo.hpp"
#include "rlSessionDetail.hpp"
#include "SessionSpoofing.hpp"
#include "Util.hpp"

#include "CommandListSessionCode.hpp"
#include "CommandListSessionMember.hpp"

namespace Stand
{
#if I_CAN_SPY
	class CommandListSessionRequestMembers : public CommandAction
	{
	public:
		explicit CommandListSessionRequestMembers(CommandList* const parent)
			: CommandAction(parent, LOC("RM_PROMPT"), {}, NOLABEL, CMDFLAG_TEMPORARY)
		{
		}

		void onClick(Click& click) final
		{
			if (parent->as<CommandListSession>()->request_members_state == RM_PROMPT)
			{
				if (click.isRegularEdition())
				{
					setMenuName(LOC("RM_CONFIRMED"));
					parent->as<CommandListSession>()->request_members_state = RM_CONFIRMED;
				}
			}
		}
	};
#endif

	void CommandListSession::onClick(Click& click)
	{
		if (checkCreateJoinAndSpecateCommands())
		{
			this->createChild<CommandLambdaActionScript>(LOC("HSTPRF"), {}, NOLABEL, [this](const Click& click)
			{
				if (auto si = getSessionInfoIfAvailable())
				{
					int network_handle[13 * 2] = { 0 };
					std::string rid_str = fmt::to_string(si->host.gamer_handle.rockstar_id);
					NETWORK::NETWORK_HANDLE_FROM_MEMBER_ID(rid_str.c_str(), (Any*)network_handle, 13);
					g_gui.inputStop();
					NETWORK::NETWORK_SHOW_PROFILE_UI((Any*)network_handle);
				}
			}, CMDFLAG_TEMPORARY);
			this->createChild<CommandListSessionCode>();
			base_offset = children.size();
		}
		CommandList::onClick(click);
	}

	void CommandListSession::onActiveListUpdate()
	{
		if (!is_active)
		{
			if (isThisOrSublistCurrentUiOrWebList())
			{
				is_active = true;
				registerScriptTickEventHandler([this]
				{
					HANDLER_CHECK(is_active)
					onTickAsActiveList();
					HANDLER_END
				});
			}
		}
		else
		{
			if (!isThisOrSublistCurrentUiOrWebList())
			{
				is_active = false;
			}
		}
	}

	void CommandListSession::onTickAsActiveList()
	{
		auto si = getSessionInfoIfAvailable();
		if (!si)
		{
			reset();
			return;
		}

		if (si->id == SessionSpoofing::getRealSessionInfo().id
			&& si->token == SessionSpoofing::getRealSessionInfo().token
			)
		{
			reset();
			return;
		}

		auto detail = RageConnector::getCachedSessionDetail(si->host.startup_id);
		if (!detail
			&& RageConnector::isConnectingTo(si->host.startup_id)
			)
		{
			if (dead == 2)
			{
				setStatus(LOC("CONFAIL"));
			}
			else
			{
				setStatus(LOC("LSESSDETS"));
			}
			return;
		}
		if (!RageConnector::hasConnection(si->host.startup_id))
		{
			RageConnector::connect(*si);
			if (dead == 0)
			{
				dead = 1;
			}
			else if (dead == 1)
			{
				dead = 2;
			}
			if (!detail)
			{
				if (getNumShownMembers() == 0)
				{
					if (dead != 2)
					{
						setStatus(LOC("LSESSDETS"));
					}
					removeRequestMembersAction();
				}
				return;
			}
		}

		if (!detail)
		{
			reset();
			return;
		}

		dead = 0;

#if !I_CAN_SPY
		setStatusToMemberCount(*detail, detail->m_NumFilledPublicSlots + detail->m_NumFilledPrivateSlots);
#else
		if (request_members_state == RM_PROMPT)
		{
			setStatusToMemberCount(*detail, detail->m_NumFilledPublicSlots + detail->m_NumFilledPrivateSlots);
			if (RageConnector::pending_join_host_startup_id != si->host.startup_id
				&& !HUD::IS_WARNING_MESSAGE_ACTIVE()
				&& !is_session_transition_active(true)
				)
			{
				if (request_members_action == nullptr)
				{
					request_members_action = this->createChild<CommandListSessionRequestMembers>();
					processChildrenUpdate();
				}
			}
			else
			{
				removeRequestMembersAction();
			}
		}
		else if (request_members_state == RM_CONFIRMED)
		{
			if (RageConnector::hasConnection(si->host.startup_id))
			{
				if (slot == -1)
				{
					slot = 0;
					if (g_hooking.override_mm_group == 4)
					{
						slot = 4;
					}
				}
				RageConnector::sendJoinRequest(si->host.startup_id);
				request_members_state = RM_SENT_JR;
			}
		}
		else if (request_members_state == RM_SENT_JR)
		{
			auto spy = RageConnector::spyFindSessionId(si->id);
			if (!spy)
			{
				slot = (slot == 0 ? 4 : 0);

				auto og = g_hooking.override_mm_group;
				g_hooking.override_mm_group = slot;
				RageConnector::sendJoinRequest(si->host.startup_id);
				g_hooking.override_mm_group = og;

				request_members_state = RM_SENT_JR_DIFF_SLOT;
			}
			else if (spy->hasEvent())
			{
				request_members_state = RM_SPYING;
				goto _process_rm_spying;
			}
		}
		else if (request_members_state == RM_SENT_JR_DIFF_SLOT)
		{
			auto spy = RageConnector::spyFindSessionId(si->id);
			if (!spy)
			{
				Util::toast(LOC("REJBLK_T"));
				resetRequestMembers();
			}
			else if (spy->hasEvent())
			{
				request_members_state = RM_SPYING;
				goto _process_rm_spying;
			}
		}
		else if (request_members_state == RM_SPYING)
		{
		_process_rm_spying:
			auto spy = RageConnector::spyFindSessionId(si->id);
			if (!spy)
			{
				if (RageConnector::hasConnection(si->host.startup_id))
				{
					auto og = g_hooking.override_mm_group;
					g_hooking.override_mm_group = slot;
					RageConnector::sendJoinRequest(si->host.startup_id);
					g_hooking.override_mm_group = og;

					request_members_state = RM_SPYING_RECONNECT;
				}
				else
				{
					resetRequestMembers();
				}
			}
			else if (RageConnector::pending_join_host_startup_id == si->host.startup_id)
			{
				spy->gracefullyStopSpying();
				resetRequestMembers();
			}
			else
			{
				while (spy->hasEvent())
				{
					auto evt = spy->popEvent();
					switch (evt.type)
					{
					case SpyEvent::ADD_GAMER:
						{
							bool found = false;
							for (auto i = children.begin() + getMembersOffset(); i != children.end(); )
							{
								if ((*i)->as<CommandListSessionMember>()->gamerInfo.getHandle().rockstar_id == evt.gamerInfo.getHandle().rockstar_id)
								{
									(*i)->as<CommandListSessionMember>()->last_accounted_for = get_current_time_millis();
									found = true;
								}
								else if (GET_MILLIS_SINCE((*i)->as<CommandListSessionMember>()->last_accounted_for) > 30000)
								{
									EXCEPTIONAL_LOCK_WRITE(g_gui.root_mtx)
									(*i)->preDelete();
									i = children.erase(i);
									processChildrenUpdate();
									EXCEPTIONAL_UNLOCK_WRITE(g_gui.root_mtx)
									continue;
								}
								++i;
							}
							if (!found)
							{
								this->createChild<CommandListSessionMember>(std::move(evt.gamerInfo));
								processChildrenUpdate();
								setStatusToMemberCount(*detail, (unsigned int)getNumShownMembers());
							}
						}
						break;

					case SpyEvent::REMOVE_GAMER:
						{
							bool found = false;
							for (auto i = children.begin() + getMembersOffset(); i != children.end(); ++i)
							{
								if ((*i)->as<CommandListSessionMember>()->gamerInfo.getHandle().rockstar_id == evt.gamerInfo.getHandle().rockstar_id)
								{
									(*i)->beGone();
									setStatusToMemberCount(*detail, (unsigned int)getNumShownMembers());
									found = true;
									break;
								}
							}
							if (!found)
							{
#ifdef STAND_DEBUG
								Util::toast("[Spy] Host removed unknown gamer");
#endif
							}
						}
						break;
					}
				}
				removeRequestMembersAction();
			}
		}
		else if (request_members_state == RM_SPYING_RECONNECT)
		{
			auto spy = RageConnector::spyFindSessionId(si->id);
			if (!spy)
			{
				resetRequestMembers();
			}
			else if (spy->hasEvent())
			{
				request_members_state = RM_SPYING;
				goto _process_rm_spying;
			}
		}
#endif
	}

	void CommandListSession::reset()
	{
		if (divider != nullptr)
		{
			divider->beGone();
			divider = nullptr;
		}
		resetRequestMembers();
	}

	void CommandListSession::resetRequestMembers()
	{
		removeRequestMembersAction();
		removeMembers();
		request_members_state = RM_PROMPT;
	}

	void CommandListSession::removeRequestMembersAction()
	{
		if (request_members_action != nullptr)
		{
			request_members_action->beGone();
			request_members_action = nullptr;
		}
	}

	void CommandListSession::removeMembers()
	{
		if (getNumShownMembers() != 0)
		{
			EXCEPTIONAL_LOCK_WRITE(g_gui.root_mtx)
			const size_t offset = getMembersOffset();
			for (auto i = children.begin() + offset; i != children.end(); )
			{
				(*i)->preDelete();
				i = children.erase(i);
			}
			children.resize(offset);
			processChildrenUpdate();
			EXCEPTIONAL_UNLOCK_WRITE(g_gui.root_mtx)
		}
	}

	void CommandListSession::setStatus(Label&& status)
	{
		if (divider == nullptr)
		{
			divider = this->createChild<CommandDivider>(std::move(status));
			processChildrenUpdate();
		}
		else
		{
			divider->setMenuName(std::move(status));
		}
	}

	void CommandListSession::setStatusToMemberCount(const rage::rlSessionDetail& detail, unsigned int count)
	{
		setStatus(LIT(fmt::format(
			fmt::runtime((detail.m_SessionConfig.m_Attrs.m_SessionPurpose == 0) ? LANG_GET("PLYCNT") : LANG_GET("PTCCNT")),
			FMT_ARG("cur", count),
			FMT_ARG("max", detail.m_SessionConfig.max_public_slots + detail.m_SessionConfig.max_private_slots)
		)));
	}

	size_t CommandListSession::getMembersOffset() const noexcept
	{
		size_t offset = base_offset;
		if (divider)
		{
			++offset;
		}
		if (request_members_action)
		{
			++offset;
		}
		return offset;
	}

	size_t CommandListSession::getNumShownMembers() const noexcept
	{
		return children.size() - getMembersOffset();
	}

	std::string CommandListSession::getGsinfo() const
	{
		if (auto si = getSessionInfoIfAvailable())
		{
			return si->toGsinfo();
		}
		return {};
	}

	int64_t CommandListSession::getSessionType() const
	{
		return GS_PUBLIC;
	}
}
