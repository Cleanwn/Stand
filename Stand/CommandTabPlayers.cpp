#include "CommandTabPlayers.hpp"

#include "CommandDivider.hpp"
#include "CommandListSelectPointer.hpp"
#include "CommandTogglePointer.hpp"
#include "Gui.hpp"

#include "CommandSelectBrackets.hpp"
#include "CommandSelectTag.hpp"

#include "CommandFocusPlayerBeaconPlural.hpp"
#include "CommandFocusPlayerBeaconSingular.hpp"
#include "CommandFocusPlayerBeaconExcludeMe.hpp"

#include "CommandStopSpectating.hpp"

#include "CommandPlayerListSort.hpp"
#include "CommandPlayerListSortReverse.hpp"
#include "CommandHoistTag.hpp"

#include "CommandAllPlayers.hpp"
#include "CommandPlayer.hpp"

namespace Stand
{
	CommandTabPlayers::CommandTabPlayers(CommandList* const parent)
		: CommandTab(parent, TAB_PLAYERS, LOC("PLYLST"), { CMDNAME("tplayers"), CMDNAME("players"), CMDNAME("playerlist") })
	{
	}

	void CommandTabPlayers::populate()
	{
		// Settings
		{
			auto settings = createChild<CommandList>(LOC("STTNGS"));

			// Tags
			{
				auto tags = settings->createChild<CommandList>(LOC("FLAGS"));

				tags->createChild<CommandSelectBrackets>();
				tags->createChild<CommandTogglePointer>(&CommandPlayer::classification_flags_on_self, LOC("CLSFNTAGSLF"));
				tags->createChild<CommandDivider>(LOC("FLAGS"));
#define FLAG_COMMAND(id, name) tags->createChild<CommandSelectTag>(LOC(name), id);
				FOR_ALL_FLAGS(FLAG_COMMAND);
			}
			
			if (g_gui.isRootStateFull())
			{
				// AR Beacon On Focused Players
				{
					auto ar_beacon = settings->createChild<CommandList>(LOC("ARBCNPLY"));

					ar_beacon->createChild<CommandFocusPlayerBeaconPlural>();
					ar_beacon->createChild<CommandFocusPlayerBeaconSingular>();
					ar_beacon->createChild<CommandFocusPlayerBeaconExcludeMe>();
				}

				// When Entering Player's List...
				{
					auto on_click = settings->createChild<CommandList>(LOC("WHNENTPLY"));

					on_click->createChild<CommandListSelectPointer>(LOC("SPCPLY"), {}, NOLABEL, std::vector<CommandListActionItemData>{
						{ 0, LOC("DOFF") },
						{ 1, LOC("JM_NUTS") },
						{ 2, LOC("LGTM") },
					}, &CommandPlayer::on_click_enable_spectate);
				}

				// When Leaving Player's List...
				{
					auto on_back = settings->createChild<CommandList>(LOC("WHNLVEPLY"));

					on_back->createChild<CommandTogglePointer>(&CommandPlayer::on_back_disable_spectate, LOC("STPSPECPLY"));
				}

				settings->createChild<CommandStopSpectating>();

				settings->createChild<CommandDivider>(LOC("SRT"));
				settings->createChild<CommandPlayerListSort>();
				settings->createChild<CommandPlayerListSortReverse>();
				auto hoist_tags = settings->createChild<CommandList>(LOC("HOISTFLGS"), {}, LOC("HOISTFLGS_H"));
#define CREATE_HOIST_CMD(id, name) hoist_tags->createChild<CommandHoistTag>(id, LOC(name));
				FOR_ALL_FLAGS(CREATE_HOIST_CMD);
			}
		}
		if (g_gui.isRootStateFull())
		{
			createChild<CommandAllPlayers>();
		}
		divider = createChild<CommandDivider>(LOC("GENWAIT"));

		CommandPhysical::collapse_command_names.emplace(L"p");
		CommandPhysical::collapse_command_names.emplace(L"apt");
	}
}
