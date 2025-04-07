#include "CommandPlayerHistory.hpp"

#include "CommandDivider.hpp"
#include "CommandLambdaAction.hpp"
#include "CommandTogglePointer.hpp"
#include "RemoteGamer.hpp"
#include "rlFriend.hpp"
#include "rockstar_admins.hpp"

#include "CommandSearchPlayer.hpp"
#include "CommandStarredPlayers.hpp"
#include "CommandTrackedPlayers.hpp"
#include "CommandWhitelistedPlayers.hpp"

#include "CommandHistoryAddName.hpp"
#include "CommandHistoryAddRid.hpp"

#include "CommandMarkedPlayers.hpp"
#include "CommandHistoryCopy.hpp"

#include "CommandHistoryAdminsNote.hpp"
#include "CommandHistoryFriendsNote.hpp"

#include "CommandClearHistory.hpp"

namespace Stand
{
	CommandPlayerHistory::CommandPlayerHistory()
		: CommandList(nullptr, LOC("PLYHIST"), { CMDNAME("history") })
	{
		createChild<CommandSearchPlayer>();
		starred = createChild<CommandStarredPlayers>();
		tracked = createChild<CommandTrackedPlayers>();
		{
			auto meta = createChild<CommandList>(LOC("META"));

			{
				meta->createChild<CommandDivider>(LOC("HISTADD2"));

				meta->createChild<CommandHistoryAddName>();
				meta->createChild<CommandHistoryAddRid>();
			}

			{
				meta->createChild<CommandDivider>(LOC("AGGREGS"));

				meta->createChild<CommandMarkedPlayers>();
				meta->createChild<CommandWhitelistedPlayers>();
				meta->createChild<CommandHistoryCopy>();
			}

			{
				meta->createChild<CommandDivider>(LOC("STTNGS"));

				{
					auto toast_config = meta->createChild<CommandList>(LOC("TRK_T_CNF"));
					RemoteGamer::tracking_update_toast_config.populateList(toast_config);
				}
				meta->createChild<CommandTogglePointer>(&RemoteGamer::inject_tracked_info, LOC("INJTRKINFO"), { CMDNAME("injecttrackedinfo"), CMDNAME("injectrackedinfo") }, NOLABEL);
			}

			{
				meta->createChild<CommandDivider>(LOC("MISC"));

				// Admins
				{
					auto admins = meta->createChild<CommandList>(LOC("RSADMINS"));

					CommandIssuable::collapse_command_names.emplace(L"admins");

					admins->createChild<CommandLambdaAction>(LOC("ADDMISS"), CMDNAMES("adminsupdate"), NOLABEL, [](Click& click)
					{
						std::unordered_set<int64_t> rids{};
						for (const auto& p : PlayerHistory::player_history)
						{
							if (is_admin_rid(p->rid))
							{
								rids.emplace(p->rid);
							}
						}
						for (const auto& e : rockstar_admins)
						{
							if (!rids.contains(e.first))
							{
								PlayerHistory::insert(e.first, e.second);
							}
						}
					});
					admins->createChild<CommandHistoryAdminsNote>();
					admins->createChild<CommandLambdaAction>(LOC("TRCKALL"), CMDNAMES("adminstrack"), NOLABEL, [](Click& click)
					{
						if (!click.isUltimateEdition())
						{
							return;
						}
						for (const auto& p : PlayerHistory::player_history)
						{
							if (is_admin_rid(p->rid))
							{
								p->track();
							}
						}
						PlayerHistory::save();
					}, CMDFLAG_FEATURELIST_ULTIMATE);
					admins->createChild<CommandLambdaAction>(LOC("UTRCKALL"), CMDNAMES("adminsuntrack"), NOLABEL, [](Click& click)
					{
						for (const auto& p : PlayerHistory::player_history)
						{
							if (is_admin_rid(p->rid))
							{
								p->untrack();
							}
						}
						PlayerHistory::save();
					});
				}

				// Tracked Players
				{
					auto track = meta->createChild<CommandList>(LOC("HISTTRK"));

					track->createChild<CommandLambdaAction>(LOC("UTRCKALL"), CMDNAMES_OBF("untracktracked"), NOLABEL, [](Click& click)
					{
						if (click.isUltimateEdition())
						{
							for (const auto& p : PlayerHistory::player_history)
							{
								if (p->isTracked())
								{
									p->untrack();
								}
							}

							PlayerHistory::save();
						}
					});

					track->createChild<CommandLambdaAction>(LOC("WHTLALL"), CMDNAMES_OBF("whitelisttracked"), NOLABEL, [](Click& click)
					{
						if (click.isUltimateEdition())
						{
							for (const auto& p : PlayerHistory::player_history)
							{
								if (p->isTracked())
								{
									p->flags |= HP_WHITELIST;
								}
							}

							PlayerHistory::save();
						}
					});

					track->createChild<CommandLambdaAction>(LOC("UWHTLALL"), CMDNAMES_OBF("unwhitelisttracked"), NOLABEL, [](Click& click)
					{
						if (click.isUltimateEdition())
						{
							for (const auto& p : PlayerHistory::player_history)
							{
								if (p->isTracked())
								{
									p->flags &= ~HP_WHITELIST;
								}
							}

							PlayerHistory::save();
						}
					});

					track->createChild<CommandLambdaAction>(LOC("MARKASFF"), CMDNAMES_OBF("fakefriendtracked"), NOLABEL, [](Click& click)
					{
						if (click.isUltimateEdition())
						{
							for (const auto& p : PlayerHistory::player_history)
							{
								if (p->isTracked())
								{
									p->setTracking(HistoricPlayer::Tracking{ 2 });
								}
							}

							PlayerHistory::save();
						}
					});

					track->createChild<CommandLambdaAction>(LOC("UMARKASFF"), CMDNAMES_OBF("unfakefriendtracked"), NOLABEL, [](Click& click)
					{
						if (click.isUltimateEdition())
						{
							for (const auto& p : PlayerHistory::player_history)
							{
								if (p->isTracked())
								{
									p->setTracking(HistoricPlayer::Tracking{ 1 });
								}
							}

							PlayerHistory::save();
						}
					});
				}

				// Friends
				{
					auto friends = meta->createChild<CommandList>(LOC("SCFRIENDS"));

					CommandIssuable::collapse_command_names.emplace(L"friends");

					friends->createChild<CommandLambdaAction>(LOC("ADDMISS"), { CMDNAME("friendsupdate") }, NOLABEL, [](Click& click)
					{
						const auto count = rage::rlFriend::getCount();
						for (int i = 0; i != count; ++i)
						{
							const auto f = rage::rlFriend::get(i);
							if (PlayerHistory::findRID(f->sc_friend.rockstar_id) == nullptr)
							{
								PlayerHistory::insert(f->sc_friend.rockstar_id, f->sc_friend.name);
							}
						}
					});

					friends->createChild<CommandHistoryFriendsNote>();

					friends->createChild<CommandLambdaAction>(LOC("TRCKALL"), { CMDNAME("friendstrack") }, NOLABEL, [](Click& click)
					{
						if (!click.isUltimateEdition())
						{
							return;
						}

						for (const auto& p : PlayerHistory::player_history)
						{
							if (p->isFriend())
							{
								p->track();
							}
						}

						PlayerHistory::save();
					}, CMDFLAG_FEATURELIST_ULTIMATE);

					friends->createChild<CommandLambdaAction>(LOC("UTRCKALL"), { CMDNAME("friendsuntrack") }, NOLABEL, [](Click& click)
					{
						if (!click.isUltimateEdition())
						{
							return;
						}

						for (const auto& p : PlayerHistory::player_history)
						{
							if (p->isFriend())
							{
								p->untrack();
							}
						}

						PlayerHistory::save();
					}, CMDFLAG_FEATURELIST_ULTIMATE);

					friends->createChild<CommandLambdaAction>(LOC("WHTLALL"), { CMDNAME("friendswhitelist") }, NOLABEL, [](Click& click)	
					{
						for (const auto& p : PlayerHistory::player_history)
						{
							if (p->isFriend())
							{
								p->flags |= HP_WHITELIST;
							}
						}

						PlayerHistory::save();
					});

					friends->createChild<CommandLambdaAction>(LOC("UWHTLALL"), { CMDNAME("friendsunwhitelist") }, NOLABEL, [](Click& click)
					{
						for (const auto& p : PlayerHistory::player_history)
						{
							if (p->isFriend())
							{
								p->flags &= ~HP_WHITELIST;
							}
						}

						PlayerHistory::save();
					});
				}

				// Danger Zone
				meta->createChild<CommandDivider>(LOC("HIST_DNG"));
				{
					auto clear = meta->createChild<CommandList>(LOC("HIST_CLR"));

					auto action = clear->createChild<CommandClearHistory>();

					action->keep_noted = clear->createChild<CommandToggle>(LOC("HIST_KEEP_NTE"), {}, NOLABEL, true);
					action->keep_tracked = clear->createChild<CommandToggle>(LOC("HIST_KEEP_TRA"), {}, NOLABEL, true);
					action->keep_whitelisted = clear->createChild<CommandToggle>(LOC("HIST_KEEP_WHT"), {}, NOLABEL, true);
					action->keep_join_reactions = clear->createChild<CommandToggle>(LOC("HIST_KEEP_JRE"), {}, NOLABEL, true);
				}
			}
		}
	}

	void CommandPlayerHistory::onClick(Click& click)
	{
		if (!PlayerHistory::inited)
		{
			click.setResponse(LOC("GENWAIT"));
			onClickImplUnavailable(click);
			return;
		}
		return CommandList::onClick(click);
	}

	bool CommandPlayerHistory::hasCommandsWhereCommandNameStartsWith(const CommandName& command_name_prefix) const noexcept
	{
		return checkCommandNameMatch(command_name_prefix, CMDNAME("history")) != NMT_MISSED
			|| checkCommandNameMatch(command_name_prefix, CMDNAME("redirect")) != NMT_MISSED
			|| checkCommandNameMatch(command_name_prefix, CMDNAME("findplayer")) > NMT_MISSED
			|| checkCommandNameMatch(command_name_prefix, CMDNAME("injecttrackedinfo")) > NMT_MISSED
			|| checkCommandNameMatch(command_name_prefix, CMDNAME("injectrackedinfo")) > NMT_MISSED
			|| checkCommandNameMatch(command_name_prefix, CMDNAME("adminsupdate")) > NMT_MISSED
			|| checkCommandNameMatch(command_name_prefix, CMDNAME("adminsnote")) > NMT_MISSED
			|| checkCommandNameMatch(command_name_prefix, CMDNAME("adminstrack")) > NMT_MISSED
			|| checkCommandNameMatch(command_name_prefix, CMDNAME("adminsuntrack")) > NMT_MISSED
			|| checkCommandNameMatch(command_name_prefix, CMDNAME("friendsupdate")) > NMT_MISSED
			|| checkCommandNameMatch(command_name_prefix, CMDNAME("friendsnote")) > NMT_MISSED
			|| checkCommandNameMatch(command_name_prefix, CMDNAME("friendstrack")) > NMT_MISSED
			|| checkCommandNameMatch(command_name_prefix, CMDNAME("friendsuntrack")) > NMT_MISSED
			|| checkCommandNameMatch(command_name_prefix, CMDNAME("friendswhitelist")) > NMT_MISSED
			|| checkCommandNameMatch(command_name_prefix, CMDNAME("friendsunwhitelist")) > NMT_MISSED
			;
	}
}
