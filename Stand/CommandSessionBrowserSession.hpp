#pragma once

#include "CommandWithOnTickFocused.hpp"
#include "CommandListSession.hpp"

#define SESSION_BROWSER_DEBUG false

#include "RageConnector.hpp"
#include "rlSessionDetail.hpp"
#include "ScAccount.hpp"
#include "Session.hpp"
#if SESSION_BROWSER_DEBUG
static_assert(STAND_DEBUG);
#include "Util.hpp"
#endif

namespace Stand
{
#pragma pack(push, 1)
	class CommandSessionBrowserSession : public CommandWithOnTickFocused<CommandListSession>
	{
	private:
		[[nodiscard]] static Label getMenuName(const SessionAttributes& attrs)
		{
			std::string name = getRegionLabel(attrs.region).getLocalisedUtf8();
			if (attrs.players != 0)
			{
				name.append("; ");
				name.append(fmt::to_string(attrs.players));
				name.push_back(' ');
				if (attrs.players == 1)
				{
					name.append(LANG_GET("PLY"));
				}
				else
				{
					name.append(LANG_GET("PLYLST"));
				}
			}
			if (!attrs.visible)
			{
				name.append("; ").append(LANG_GET("HIDE"));
			}
			if (attrs.bad_sport)
			{
				name.append("; ").append(LANG_GET("MMPOOL_BADBOY"));
			}
			return LIT(std::move(name));
		}

	public:
		Session sess;

		explicit CommandSessionBrowserSession(CommandList* const parent, const rage::rlSessionInfo& info, const SessionAttributes& attrs)
			: CommandWithOnTickFocused(parent, getMenuName(attrs), {}, NOLABEL, CMDFLAGS_LIST | CMDFLAG_SEARCH_FINISHLIST | CMDFLAG_FEATURELIST_SKIP | CMDFLAG_TEMPORARY), sess{ info, attrs }
		{
		}

		[[nodiscard]] const rage::rlSessionInfo* getSessionInfoIfAvailable() const final;
		bool checkCreateJoinAndSpecateCommands() final;

		void onTickInGameViewport() final
		{
			CommandWithOnTickFocused::onTickInGameViewport();

			if (RageConnector::use_speculative_session_preconnections
				&& RageConnector::getCachedSessionDetail(sess.info.host.startup_id) == nullptr
				)
			{
				RageConnector::connect(sess.info);
			}
		}

		void onTickFocused() final
		{
			if (!isHelpTextShown())
			{
				return;
			}
			auto host = ScAccount::fromRID(sess.info.host.gamer_handle.rockstar_id);
			std::string host_name = host->toString(false);
			if (!host->isComplete())
			{
				if (auto detail = RageConnector::getCachedSessionDetail(sess.info.host.startup_id))
				{
					host_name = detail->m_HostName;
				}
				else if (!RageConnector::hasConnection(sess.info.host.startup_id)
					&& !RageConnector::isConnectingTo(sess.info.host.startup_id)
					&& ScAccount::canRequestCompletionImmediately()
					)
				{
					host->requestCompletion();
				}
			}
			std::string help;
#if SESSION_BROWSER_DEBUG
			help.append("ID: ").append(Util::to_padded_hex_string(sess.info.id)).push_back('\n');
			help.append("Token: ").append(Util::to_padded_hex_string(sess.info.token)).push_back('\n');
#endif
			help.append(LANG_GET("FLAG_HOST"));
			help.append(": ");
			help.append(ScAccount::formatName(sess.info.host.gamer_handle.rockstar_id, host_name));
			help.push_back('\n');
			help.append(LANG_GET("AIM_MODE_3"));
			help.append(": ");
			help.append(sess.attrs.free_aim ? LANG_GET("YES") : LANG_GET("NO"));
			setHelpText(LIT(std::move(help)));
		}

		[[nodiscard]] static Label getRegionLabel(uint8_t region_code) noexcept
		{
			switch (region_code)
			{
			case 5: return LOC("Australia");
			case 3: return LOC("Europe");
			case 4: return LOC("Asia");
			case 7: return LOC("Japan");
			case 1: return LOC("LATAM");
			case 0: return LOC("REGION_NA");
			case 2: return LOC("USA_East");
			case 6: return LOC("USA_West");
			}
			return NOLABEL;
		}
	};
#pragma pack(pop)
}
