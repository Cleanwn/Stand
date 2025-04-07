#include "CommandSessionBrowser.hpp"

#include "Auth.hpp"
#include "Blacklist.hpp"
#include "CNetworkAssetVerifier.hpp"
#include "CommandDivider.hpp"
#include "CommandLambdaListSelect.hpp"
#include "CommandSlider.hpp"
#include "CommandSliderPointer.hpp"
#include "CommandTogglePointer.hpp"
#include "Hooking.hpp"
#include "Matchmaking.hpp"
#include "pointers.hpp"
#include "Session.hpp"
#include "StringUtils.hpp"

#ifdef STAND_DEBUG
#include "drawDebugText.hpp"
#endif

#include "CommandSessionBrowserRefresh.hpp"
#include "CommandSessionBrowserSession.hpp"

namespace Stand
{
	CommandSessionBrowser::CommandSessionBrowser(CommandList* const parent)
		: CommandListWithOnTickAsActiveList(parent, LOC("SESSBRWS"), {}, NOLABEL, CMDFLAGS_LIST | CMDFLAG_FEATURELIST_REGULAR)
	{
		//this->createChild<CommandDivider>(LOC("STTNGS"));
		this->createChild<CommandSliderPointer<CommandSlider, compactplayer_t>>(LOC("MINPLYS"), { CMDNAME("browserminplayers") }, NOLABEL, &config.min_players, 0, MAX_PLAYERS - NUM_SPECTATOR_SLOTS);
		this->createChild<CommandSliderPointer<CommandSlider, compactplayer_t>>(LOC("MAXPLYS"), { CMDNAME("browsermaxplayers") }, NOLABEL, &config.max_players, 1, MAX_PLAYERS - NUM_SPECTATOR_SLOTS);
		this->createChild<CommandLambdaListSelect>(LOC("MMREGION"), {}, NOLABEL, [this](long long value, Click&)
		{
			this->config.region = (uint32_t)value;
		}, std::vector<CommandListActionItemData>{
			{ -2, LOC("NFLTR") },
			{ -1, LOC("OVRDOFF") },
			{ 5, LOC("Australia") },
			{ 3, LOC("Europe") },
			{ 4, LOC("Asia") },
			{ 7, LOC("Japan") },
			{ 1, LOC("LATAM") },
			{ 0, LOC("REGION_NA") },
			{ 2, LOC("USA_East") },
			{ 6, LOC("USA_West") },
		}, -2);
		this->createChild<CommandLambdaListSelect>(LOC("AIM_MODE_3"), {}, NOLABEL, [this](long long value, Click&)
		{
			this->config.free_aim = (uint8_t)value;
		}, std::vector<CommandListActionItemData>{
			{ 0xFF, LOC("NFLTR") },
			{ 1, LOC("YES") },
			{ 0, LOC("NO") },
		}, 0xFF);

		{
			auto more = this->createChild<CommandList>(LOC("MREFLTR"));

			more->createChild<CommandLambdaListSelect>(LOC("HIDE"), {}, NOLABEL, [this](long long value, Click&)
			{
				this->config.visible = (uint8_t)value;
			}, std::vector<CommandListActionItemData>{
				{ 0xFF, LOC("NFLTR") },
				{ 1, LOC("NO") },
				{ 0, LOC("YES") },
			}, 1);
			more->createChild<CommandLambdaListSelect>(LOC("MMPOOL_BADBOY"), {}, NOLABEL, [this](long long value, Click&)
			{
				this->config.bad_sport = (uint8_t)value;
			}, std::vector<CommandListActionItemData>{
				{ 0xFF, LOC("NFLTR") },
				{ 1, LOC("YES") },
				{ 0, LOC("NO") },
			}, 0xFF);
			more->createChild<CommandTogglePointer>(&this->config.filter_user, LOC("AST"), {}, LOC("SESSBRWS_FLTR_AST_H"));
			more->createChild<CommandTogglePointer>(&this->config.adblock, LOC("NHKNADS"));
		}

		this->createChild<CommandSessionBrowserRefresh>();
		results_divider = this->createChild<CommandDivider>(LOC("GENWAIT"));
		results_offset = children.size();
	}

	size_t CommandSessionBrowser::getNumSessions() const noexcept
	{
		return children.size() - results_offset;
	}

	void CommandSessionBrowser::onClick(Click& click)
	{
		if (!click.isRegularEdition())
		{
			return;
		}
		if (true /* !g_hooking.rage_rlScMatchmaking_Find_hook.isHooked() || !g_hooking.rage_rlScMatchmakingFindTask_ReadSession_hook.isHooked() */)
		{
			return click.setResponse(LIT(LANG_FMT("HKFAIL", LANG_GET("SESSBRWS"))));
		}
		CommandListWithOnTickAsActiveList::onClick(click);
	}

	[[nodiscard]] static uint32_t calculateMatchmakingUserValue()
	{
		uint32_t user = 0;

		uint32_t ecm_crc = hooks::CExtraContentManager_GetCRC(*pointers::extra_content_manager, 0);
		user = rage::atDataHash((uint32_t*)&ecm_crc, sizeof(uint32_t), user);

		uint32_t pool = 0xC43A73D * hooks::get_matchmaking_pool();
		user = rage::atDataHash((uint32_t*)&pool, sizeof(uint32_t), user);

		uint32_t timeout_value = 8000;
		user = rage::atDataHash((uint32_t*)&timeout_value, sizeof(uint32_t), user);

		uint32_t av_crc = (*pointers::asset_verifier)->m_CRC.Get();
		user = rage::atDataHash((uint32_t*)&av_crc, sizeof(uint32_t), user);

		user %= 0x3FFF;

		return user;
	}

	void CommandSessionBrowser::onTickAsActiveList()
	{
		if (request_sent)
		{
#ifdef STAND_DEBUG
			// there might be some weirdness here when the game is in offline mode (no internet)
			drawDebugText(Matchmaking::status.m_Status);
#endif
			if (Matchmaking::status.m_StatusCode == rage::netStatus::SUCCEEDED)
			{
				size_t new_sesss = 0;
				Matchmaking::consumeResults([this, &new_sesss](hash_t id_hash, const rage::rlSessionInfo& info)
				{
					if (!discovered_sessions.contains(id_hash))
					{
						++new_sesss;
						discovered_sessions.emplace(id_hash);

						const SessionAttributes& attrs = Matchmaking::getSessionAttributes(id_hash);
						if (attrs.players >= results_config.min_players
							&& attrs.players <= results_config.max_players
							&& (results_config.free_aim == 0xFF || attrs.free_aim == (bool)results_config.free_aim)
							&& (results_config.visible == 0xFF || attrs.visible == (bool)results_config.visible)
							&& (results_config.bad_sport == 0xFF || attrs.bad_sport == (bool)results_config.bad_sport)
							&& (!results_config.filter_user || attrs.user == results_config.user)
							&& (!results_config.adblock || !Blacklist::isAdvertiserByRid(info.host.gamer_handle.rockstar_id))
							)
						{
							total_players += (attrs.players
								+ (attrs.players == 0) // ensure we count at least 1
								);
							this->createChild<CommandSessionBrowserSession>(info, attrs);
						}
					}
				});
				if (new_sesss < 100)
				{
					diminishing_returns = true;
				}
				Matchmaking::clear();
				processChildrenUpdate();

				auto str = StringUtils::toStringWithThousandsSeparator(getNumSessions());
				if (!diminishing_returns)
				{
					str.push_back('+');
				}
				str.push_back(' ');
				str.append(LANG_GET("SESSS"));
				str.append("; ");
				if (diminishing_returns)
				{
					str.push_back('~');
				}
				str.append(StringUtils::toStringWithThousandsSeparator(total_players));
				if (!diminishing_returns)
				{
					str.push_back('+');
				}
				str.push_back(' ');
				str.append(LANG_GET("PLYLST"));
				results_divider->setMenuName(LIT(std::move(str)));

				request_sent = false;
			}
			else if (Matchmaking::status.m_StatusCode == rage::netStatus::FAILED)
			{
				request_sent = false;
			}
			return;
		}

		if (config.filter_user)
		{
			config.user = calculateMatchmakingUserValue();
		}

		if (results_config != config /* config changed? */
			|| want_refresh /* user pressed refresh or first time visiting list? */
			)
		{
			discovered_sessions.clear();
			total_players = 0;
			diminishing_returns = false;
			children.resize(results_offset);
			processChildrenUpdate();
			results_divider->setMenuName(LOC("GENWAIT"));
		}
		else if (diminishing_returns)
		{
			return;
		}
		if (!Matchmaking::canSendRequest()
			|| g_auth.license_permissions < LICPERM_REGULAR
			)
		{
			return;
		}
		results_config = config;
		request_sent = true;
		want_refresh = false;
		Matchmaking::sendRequest(results_config);
	}
}
