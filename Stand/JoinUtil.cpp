#include "JoinUtil.hpp"

#include <soup/Bytepatch.hpp>
#include <soup/ObfusString.hpp>

#include "Exceptional.hpp"
#include "FiberPool.hpp"
#include "is_rid.hpp"
#include "get_next_arg.hpp"
#include "Gui.hpp"
#include "Hooking.hpp"
#include "HttpRequestBuilder.hpp"
#include "natives.hpp"
#include "NativeTableHooks.hpp"
#include "pointers.hpp"
#include "RageConnector.hpp"
#include "RemoteGamer.hpp"
#include "rlSessionDetail.hpp"
#include "Script.hpp"
#include "session_code_cache.hpp"
#include "SessionSpoofing.hpp"
#include "set_aim_mode.hpp"
#include "StringUtils.hpp"
#include "tbLoadingSessionDetails.hpp"
#include "TranscendentVehicle.hpp"
#include "TransitionHelper.hpp"
#include "Util.hpp"

#ifdef STAND_DEBUG
#include "drawDebugText.hpp"
#endif

namespace Stand
{
	void JoinUtil::connectViaCode(const std::string& code, bool as_spectator)
	{
		auto entry = session_code_cache.find(code);
		if (entry == session_code_cache.end())
		{
			g_tb_loading_session_details.enable();
			std::string path(soup::ObfusString("/api/session_code_redeem?code=").str());
			path.append(code);
			HttpRequestBuilder(HttpRequestBuilder::GET, soup::ObfusString("stand.sh"), std::move(path))
				.setResponseCallback([code, as_spectator](soup::HttpResponse&& resp)
				{
					g_tb_loading_session_details.disable();
					if (resp.body.empty())
					{
						Util::toast(LOC("SESSCODE_T_X"));
					}
					else
					{
						connect(resp.body, as_spectator);
						session_code_cache.emplace(code, std::move(resp.body));
					}
				})
				.setFailCallback([](std::string&&)
				{
					g_tb_loading_session_details.disable();
				})
				.dispatch();
		}
		else
		{
			connect(entry->second, as_spectator);
		}
	}

	void JoinUtil::connect(long long method, JoinHint&& hint, bool as_spectator)
	{
		if (RageConnector::pending_join_host_startup_id != 0)
		{
			return;
		}
		FiberPool::queueJob([method, hint{std::move(hint)}, as_spectator]() mutable
		{
			dispatch(method, hint, as_spectator);
		});
	}

	void JoinUtil::dispatch(long long method, JoinHint& hint, bool as_spectator)
	{
		switch (method)
		{
		case JM_NINJA: dispatchNinja(hint, as_spectator); RageConnector::pending_join_host_startup_id = 0; return;
		case JM_FRIEND: dispatchFriend(hint, as_spectator); RageConnector::pending_join_host_startup_id = 0; return;
		case JM_MATCHMAKING: dispatchMatchmaking(hint, as_spectator); RageConnector::pending_join_host_startup_id = 0; return;
		case JM_NUTS: dispatchNuts(hint, as_spectator, false); RageConnector::pending_join_host_startup_id = 0; return;
		case JM_NUTS_REJOIN: dispatchNuts(hint, as_spectator, true); RageConnector::pending_join_host_startup_id = 0; return;
		}
		Exceptional::report("Logic Error");
	}

	bool JoinHint::ensureSessionInfo(bool silent)
	{
		if (!hasSessionInfo())
		{
			auto* g = RemoteGamer::get(rid, true, RGU_BULK);
			if (g == nullptr)
			{
				if (!silent)
				{
					Util::toast(LOC("GENFAIL"));
				}
				return false;
			}
			g_tb_loading_session_details.enable();
			while (g->isQueuedOrPending())
			{
				Script::current()->yield();
			}
			g_tb_loading_session_details.disable();

			if (!g->hasSession())
			{
				if (!silent)
				{
					Util::toast(LOC("JOIN_T_N"));
				}
				return false;
			}

			session_info = g->session_info;
		}

		return true;
	}

	bool JoinHint::onPreJoin(bool method_has_no_dialogs, bool rejoin)
	{
		const bool auto_switch_targeting = (method_has_no_dialogs || JoinUtil::auto_switch_targeting);
#if I_CAN_SPY
		const bool rejectblock = JoinUtil::rejectblock;
#endif

		if (auto_switch_targeting
#if I_CAN_SPY
			|| rejectblock
#endif
			)
		{
			if (ensureSessionInfo(true))
			{
				if (SessionSpoofing::getRealSessionInfo().id != 0
					&& session_info.id == SessionSpoofing::getRealSessionInfo().id
					&& session_info.token == SessionSpoofing::getRealSessionInfo().token
					)
				{
					if (rejoin)
					{
#if I_CAN_SPY
						CONST_ASSIGN(rejectblock, false);
#endif
					}
					else
					{
						Util::toast(LANG_GET("JOIN_T_A"));
						return false;
					}
				}

				RageConnector::pending_join_host_startup_id = session_info.host.startup_id;

				if (auto_switch_targeting
#if I_CAN_SPY
					|| rejectblock
#endif
					)
				{
					RageConnector::connect(session_info);
				}
			}
		}

		if (hasSessionInfo())
		{
			RageConnector::pending_join_host_startup_id = session_info.host.startup_id;

			bool enabled_spinner = false;

			// If we are already establishing a connection to the host, wait for that to finish.
			// We can then reuse the session detail.
			if (RageConnector::isConnectingTo(session_info.host.startup_id))
			{
				g_tb_loading_session_details.enable();
				enabled_spinner = true;

				do
				{
#ifdef STAND_DEBUG
					drawDebugText("Connecting to host...");
#endif
					Script::current()->yield();
				} while (RageConnector::isConnectingTo(session_info.host.startup_id));
			}

			if (auto_switch_targeting)
			{
				if (auto detail = RageConnector::getCachedSessionDetail(session_info.host.startup_id))
				{
					const uint32_t discriminator = detail->m_SessionConfig.m_Attrs.m_Vals[0]; // assuming the discriminator is at index 0
					const bool free_aim = ((discriminator >> 17) & 0b1);
					const bool local_free_aim = (PAD::GET_LOCAL_PLAYER_GAMEPAD_AIM_STATE() >= 2);
					if (free_aim != local_free_aim)
					{
						if (free_aim)
						{
							set_aim_mode(3);
						}
						else
						{
							set_aim_mode(0);
						}
						Util::toast(LANG_FMT("AUTOCHNGTRGT_T", free_aim ? LANG_GET("AUTOCHNGTRGT_T_1") : LANG_GET("AUTOCHNGTRGT_T_0")));
					}
				}
			}

#if I_CAN_SPY
			if (RageConnector::spyFindSessionId(session_info.id))
			{
				if (!enabled_spinner)
				{
					g_tb_loading_session_details.enable();
					enabled_spinner = true;
				}

				do
				{
#ifdef STAND_DEBUG
					drawDebugText("Waiting for spy to stop...");
#endif
					Script::current()->yield();
				} while (RageConnector::spyFindSessionId(session_info.id));
			}

			if (rejectblock)
			{
				if (!enabled_spinner)
				{
					g_tb_loading_session_details.enable();
					enabled_spinner = true;
				}

				// Send join request
				RageConnector::sendJoinRequest(session_info.host.startup_id);

				// Wait for host to react
				Spy* spy;
				time_t t = get_current_time_millis();
				while (true)
				{
#ifdef STAND_DEBUG
					drawDebugText("Waiting for host to react...");
#endif
					Script::current()->yield();

					spy = RageConnector::spyFindSessionId(session_info.id);
					if (!spy || spy->hasEvent())
					{
						break;
					}

					// Some menus (Midnight) have a really bad block join that just leaks the connection.
					if (GET_MILLIS_SINCE(t) > 20000)
					{
						RageConnector::stopSpying(spy);
					}
				}

				// Handle failure
				if (!spy)
				{
					Util::toast(LOC("REJBLK_T"));
					if (enabled_spinner)
					{
						g_tb_loading_session_details.disable();
					}
					return false;
				}

				if (auto spy = RageConnector::spyFindSessionId(session_info.id))
				{
					spy->gracefullyStopSpying();
					do
					{
#ifdef STAND_DEBUG
						drawDebugText("Disconnecting...");
#endif
						Script::current()->yield();
					} while (RageConnector::spyFindSessionId(session_info.id));
				}
			}
#endif

			if (enabled_spinner)
			{
				g_tb_loading_session_details.disable();
			}
		}

		// Give seamless a head-start on saving our vehicle in case the game deletes it too quickly
		TranscendentVehicle::saveFromPlayerState();

		return true;
	}

	void JoinUtil::dispatchNinja(JoinHint& hint, bool as_spectator)
	{
		if (hint.ensureSessionInfo()
			&& hint.onPreJoin()
			)
		{
			g_hooking.managed_invite = true;
			g_hooking.managed_join = true;
			g_hooking.join_as_spectator = as_spectator;

			rage::rlGamerHandle gh{};
			pointers::InviteMgr_HandleJoinRequest(pointers::invite_mgr, &hint.session_info, pointers::rlPresence_m_ActingGamerInfo, &gh, 3);
		}
	}

	void JoinUtil::dispatchFriend(JoinHint& hint, bool as_spectator)
	{
		const auto rid = hint.getRid();

		if (!is_rid_valid(rid))
		{
			Util::toast(LOC("GENFAIL"));
			return;
		}

		if (!hint.onPreJoin())
		{
			return;
		}

		g_hooking.replacement_rid = rid;

		auto menu_ptr = fiberOpenFriendsList();
		hash_t click_hash = (as_spectator ? -865413158 : -632792895);
		g_hooking.managed_invite = true;
		g_hooking.managed_join = true;
		do
		{
			g_hooking.replace_friends_rid = true;
			auto res = pointers::CPlayerListMenu_HandleContextOption(menu_ptr, &click_hash);
			g_hooking.replace_friends_rid = false;
			if (res)
			{
				break;
			}
			Script::current()->yield();
		} while (true);
	}

	void JoinUtil::dispatchMatchmaking(JoinHint& hint, bool as_spectator)
	{
		SOUP_ASSERT_UNREACHABLE;

		if (hint.ensureSessionInfo()
			&& hint.onPreJoin(true)
			)
		{
			g_hooking.matchmaking_join = true;
			g_hooking.matchmaking_join_session = hint.session_info;
			TransitionHelper::doTransition(as_spectator ? STAND_SCTV : FIND_PUBLIC);
		}
	}

	static rage::rlSessionInfo deez_session_info;

	void JoinUtil::dispatchNuts(JoinHint& hint, bool as_spectator, bool rejoin)
	{
		SOUP_ASSERT_UNREACHABLE;

		if (hint.ensureSessionInfo()
			&& hint.onPreJoin(true, rejoin)
			)
		{
			deez_session_info = std::move(hint.session_info);
			NativeTableHooks::createNativeEntrypointOverride(0x6F3D4ED9BEE4E61D, [](rage::scrNativeCallContext& ctx)
			{
				//NETWORK::NETWORK_SET_ACTIVITY_SPECTATOR(deez_as_spectator);

				// Changed nJoinFlags from 7 to 3 for b2824
				//OG(CNetworkSession_JoinSession)(*pointers::network_session, &deez_session_info, 1, 3, nullptr, 0);

				ctx.setReturnValue<>(TRUE);
			});
			TransitionHelper::doTransition(as_spectator ? STAND_NEW_PUBLIC_AS_SPECTATOR : NEW_PUBLIC);
			NativeTableHooks::removeNativeEntrypointOverride(0x6F3D4ED9BEE4E61D);
		}
	}

	bool JoinUtil::inviteViaRidPreflightCheck(Click& click)
	{
		if (!click.inOnline())
		{
			return false;
		}
		if (NETWORK::NETWORK_SESSION_IS_SOLO())
		{
			click.setResponse(LOC("PLYINV_E"));
			return false;
		}
		return true;
	}

	void JoinUtil::inviteViaRid(Click& click, int64_t rid)
	{
		if (inviteViaRidPreflightCheck(click))
		{
			click.ensureYieldableScriptThread([rid]
			{
				inviteViaRid(rid);
			});
		}
	}

	void JoinUtil::inviteViaRid(int64_t rid)
	{
		soup::Bytepatch patch_1{};
		soup::Bytepatch patch_2{};

		uint8_t patch_1_bytes[] = {
			soup::Bytepatch::JMP1
		};
		patch_1.initPatch(pointers::CPlayerListMenu_handler_jnz, patch_1_bytes, COUNT(patch_1_bytes));

		patch_2.initPatchNOP(pointers::CPlayerListMenu_handler_jz, 2);

		*pointers::rlGamerHandle_buffer = rid;

		__try
		{
			auto menu_ptr = fiberOpenFriendsList();
			hash_t contextOption = 0xE1E8D5DC;
			pointers::CPlayerListMenu_HandleContextOption(menu_ptr, &contextOption);
			Script::current()->yield(50);
		}
		__EXCEPTIONAL()
		{
		}

		patch_1.restore();
		patch_2.restore();

		while (HUD::IS_PAUSE_MENU_ACTIVE())
		{
			HUD::SET_FRONTEND_ACTIVE(false);
			Script::current()->yield();
		}
	}

	std::string JoinUtil::parseCode(std::wstring& args)
	{
		auto code = get_next_arg(args);
		{
			auto last_slash = code.find_last_of(L'/');
			if (last_slash != std::string::npos)
			{
				code.erase(0, last_slash + 1);
			}
		}
		{
			auto last_hash = code.find_last_of(L'#');
			if (last_hash != std::string::npos)
			{
				code.erase(0, last_hash + 1);
			}
		}
		return StringUtils::utf16_to_utf8(code);
	}

	CPlayerListMenu* JoinUtil::fiberOpenFriendsList()
	{
		g_gui.inputStop();
		while (HUD::IS_PAUSE_MENU_ACTIVE())
		{
			HUD::SET_FRONTEND_ACTIVE(false);
			Script::current()->yield();
		}
		HUD::ACTIVATE_FRONTEND_MENU(0xD528C7E2, 0, 2);
		void* menu_ptr;
		while ((menu_ptr = *(void**)(*pointers::menu_ptr + (80 * 2))) == nullptr)
		{
			Script::current()->yield();
		}
		Script::current()->yield();
		return reinterpret_cast<CPlayerListMenu*>(menu_ptr);
	}
}
