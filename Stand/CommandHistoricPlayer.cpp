#include "CommandHistoricPlayer.hpp"

#include <fmt/core.h>

#include "CommandHistoricPlayerCopy.hpp"
#include "CommandHistoricPlayerJoinReactions.hpp"
#include "CommandHistoricPlayerNote.hpp"
#include "CommandHistoricPlayerSession.hpp"
#include "CommandHistoricPlayerStats.hpp"
#include "CommandHistoricPlayerTime.hpp"
#include "CommandHistoricPlayerTrack.hpp"
#include "CommandHistoricPlayerWhitelist.hpp"
#include "CommandHistoryDelete.hpp"
#include "CommandLambdaAction.hpp"
#include "CommandLambdaActionScript.hpp"
#include "CommandReadonlyName.hpp"
#include "Gui.hpp"
#include "is_rid.hpp"
#include "JoinUtil.hpp"
#include "lang.hpp"
#include "natives.hpp"
#include "PlayerHistory.hpp"
#include "RageConnector.hpp"
#include "RemoteGamer.hpp"
#include "ScAccount.hpp"
#include "StringUtils.hpp"
#include "Util.hpp"

#ifdef STAND_DEBUG
//#include "CommandHistoryTransInvite.hpp"
//#include "CommandHistoryIp.hpp"
#include "CommandReadonlyValueLambda.hpp"
#include "GamerIdentifier.hpp"
#endif

namespace Stand
{
	CommandHistoricPlayer::CommandHistoricPlayer(HistoricPlayer* const player)
		: CommandWithOnTickFocused(PlayerHistory::player_history_command, LIT(player->getNameForUser()), {}, NOLABEL, CMDFLAGS_LIST | CMDFLAG_TEMPORARY | CMDFLAG_STATE_AND_CHECK_FINISHLIST | CMDFLAG_SEARCH_FINISHLIST | CMDFLAG_FEATURELIST_SKIP | CMDFLAG_HOTKEYS_FINISHLIST | CMDFLAG_STARRED_FINISHLIST, COMMAND_LIST_HISTORICPLAYER), player(player)
	{
		help_text = getHelpText();

		CommandName simple_name = utf8ToCmdName(player->name);
		StringUtils::simplifyCommandName(simple_name);

		this->createChild<CommandHistoricPlayerNote>(simple_name);
		this->createChild<CommandHistoricPlayerCopy>(simple_name);
		auto join_reactions = this->createChildWithBuilder<CommandHistoricPlayerJoinReactions>();
		join_reactions->block_join_command_names = std::vector<CommandName>(1, std::move(CommandName(CMDNAME("historyblock")).append(simple_name)));

		this->createChild<CommandHistoricPlayerSession>(simple_name);
		this->createChild<CommandLambdaAction>(LOC("PLYINV"), std::vector<CommandName>(1, std::move(CommandName(CMDNAME("historyinvite")).append(simple_name))), NOLABEL, [this](Click& click)
		{
			click.stopInputIfAllowed();
			JoinUtil::inviteViaRid(click, this->player->rid);
		}, CMDFLAG_TEMPORARY);
#ifdef STAND_DEBUG
		//this->createChild<CommandHistoryTransInvite>();
#endif
		this->createChild<CommandLambdaActionScript>(LOC("PLYPRFL"), std::vector<CommandName>(1, std::move(CommandName(CMDNAME("historyprofile")).append(simple_name))), NOLABEL, [this](const Click& click)
		{
			if (is_rid_valid(this->player->rid))
			{
				int network_handle[13 * 2] = { 0 };
				NETWORK::NETWORK_HANDLE_FROM_MEMBER_ID(fmt::to_string(this->player->rid).c_str(), (Any*)network_handle, 13);
				g_gui.prepareForOverlay();
				NETWORK::NETWORK_SHOW_PROFILE_UI((Any*)network_handle);
			}
		}, CMDFLAG_TEMPORARY);
		this->createChild<CommandLambdaActionScript>(LOC("PLYFRND"), std::vector<CommandName>(1, std::move(CommandName(CMDNAME("historybefriend")).append(simple_name))), NOLABEL, [this](const Click& click)
		{
			if (is_rid_valid(this->player->rid))
			{
				int network_handle[13 * 2] = { 0 };
				NETWORK::NETWORK_HANDLE_FROM_MEMBER_ID(fmt::to_string(this->player->rid).c_str(), (Any*)network_handle, 13);
				g_gui.prepareForOverlay();
				NETWORK::NETWORK_ADD_FRIEND((Any*)network_handle, "");
			}
		}, CMDFLAG_TEMPORARY);
	}

	void CommandHistoricPlayer::populateCommandsWithoutCommandNames()
	{
		this->children.insert(this->children.begin() + 1, this->makeChild<CommandHistoricPlayerTrack>());
		this->children.insert(this->children.begin() + 2, this->makeChild<CommandHistoricPlayerWhitelist>());
		this->children.insert(this->children.begin() + 6, this->makeChild<CommandHistoricPlayerStats>());

		this->createChild<CommandHistoricPlayerTime>(LOC("1SEEN"), &HistoricPlayer::first_seen);
		this->createChild<CommandHistoricPlayerTime>(LOC("LSEEN"), &HistoricPlayer::last_seen);
		this->createChild<CommandLambdaAction>(LOC("CPYNME"), std::vector<CommandName>(0), NOLABEL, [this](Click& click)
		{
			Util::copy_to_clipboard(click, this->player->name);
		}, CMDFLAG_TEMPORARY);
		this->createChild<CommandLambdaAction>(LOC("CPYRID"), std::vector<CommandName>(0), NOLABEL, [this](Click& click)
		{
			Util::copy_to_clipboard(click, this->player->rid);
		}, CMDFLAG_TEMPORARY);
		this->createChild<CommandHistoryDelete>();
#ifdef STAND_DEBUG
		{
			auto secret = this->createChild<CommandList>(LIT("Debug Build Shit"), {}, NOLABEL, CMDFLAG_TEMPORARY | CMDFLAG_FEATURELIST_SKIP);

			secret->createChild<CommandReadonlyValueLambda>(LIT("GIv4"), [player{ player }]()
			{
				if (is_rid_valid(player->rid))
				{
					return StringUtils::utf8_to_utf16(GamerIdentifierV4::fromRockstarId(player->rid).toString());
				}
				return LANG_GET_W("NA");
			}, CMDFLAG_TEMPORARY);
			/*secret->createChild<CommandReadonlyValueLambda>(LIT("GIv3"), [player{ player }]()
			{
				if (is_rid_valid(player->rid))
				{
					return StringUtils::utf8_to_utf16(GamerIdentifierV3::fromRockstarId(player->rid).toString());
				}
				return LANG_GET_W("NA");
			}, CMDFLAG_TEMPORARY);*/
			secret->createChild<CommandReadonlyValueLambda>(LIT("Account ID"), [player{ player }]()
			{
				return fmt::to_wstring(player->account_id);
			}, CMDFLAG_TEMPORARY);
			secret->createChild<CommandReadonlyValueLambda>(LIT("MAC ID"), [player{ player }]()
			{
				return StringUtils::utf8_to_utf16(Util::to_padded_hex_string(player->mac_id));
			}, CMDFLAG_TEMPORARY);
			//secret->createChild<CommandHistoryIp>(player);
			secret->createChild<CommandReadonlyValueLambda>(LIT("Flags"), [player{ player }]()
			{
				std::wstring flags{};
				if (player->flags & HP_SCNAME)
				{
					StringUtils::list_append(flags, L"SC Name");
				}
				return flags;
			}, CMDFLAG_TEMPORARY);
		}
#endif
	}

	void CommandHistoricPlayer::onPreScriptedAccess()
	{
		if (!player.getBool())
		{
			player.setBool(true);
			populateCommandsWithoutCommandNames();
		}
	}

	void CommandHistoricPlayer::onActiveListUpdate()
	{
		CommandList::onActiveListUpdate();
		onPreScriptedAccess();
	}

	void CommandHistoricPlayer::onClick(Click& click)
	{
		onPreScriptedAccess();
		if (RageConnector::use_speculative_session_preconnections)
		{
			auto* g = RemoteGamer::get(player->rid, false, RGU_NONE);
			if (g && !g->isQueuedOrPending() && g->hasSession())
			{
				RageConnector::connect(g->session_info);
			}
		}
		return CommandWithOnTickFocused::onClick(click);
	}

	RemoteGamer* CommandHistoricPlayer::update()
	{
		auto name = player->getNameForUser();
		auto* g = RemoteGamer::get(player->rid, true, RGU_DETAILED);
		if (g != nullptr)
		{
			auto str = g->toShortString();
			if (!str.empty())
			{
				name.append(" [").append(str.getLocalisedUtf8()).push_back(']');
			}
		}
		setMenuName(LIT(std::move(name)));
		return g;
	}

	void CommandHistoricPlayer::onTickInGameViewport()
	{
		update();
		return CommandWithOnTickFocused::onTickInGameViewport();
	}

	void CommandHistoricPlayer::onTickFocused()
	{
		setHelpText(getHelpText());
	}

	Label CommandHistoricPlayer::getHelpText()
	{
		std::string str{};
		if (player->isTracked())
		{
			str = LANG_GET("TRKD");
		}
		if (player->isFakeFriend())
		{
			StringUtils::list_append(str, LANG_GET("FF"));
		}
		if (!player->note.empty())
		{
			if (!str.empty())
			{
				if (/*player->isTracked() &&*/ player->isFakeFriend())
				{
					str.push_back('\n');
				}
				else
				{
					str.append(", ");
				}
			}
			str.append(LANG_GET("PLYNOTE")).append(": ").append(player->note);
		}
		auto* g = RemoteGamer::get(player->rid, false, RGU_NONE);
		if (g != nullptr)
		{
			if (!str.empty())
			{
				str.push_back('\n');
			}
			str.append(g->toUserString(focused && isHelpTextShown()));
		}
		return LIT(std::move(str));
	}

	std::string CommandHistoricPlayer::getNameForConfig() const
	{
		return player->name;
	}

	std::string CommandHistoricPlayer::getState() const
	{
		return getStateImpl(this);
	}

	std::string CommandHistoricPlayer::getStateImpl(const CommandHistoricPlayer* _this)
	{
		if (RemoteGamer::haveTrackPermissions())
		{
			auto* g = RemoteGamer::get(_this->player->rid, true, RGU_BULK);
			return g->toShortString().getEnglishUtf8();
		}
		return {};
	}
}
