#pragma once

#include "CommandSlider.hpp"

#include "FiberPool.hpp"
#include "ScriptHostUtil.hpp"

namespace Stand
{
	class CommandCeoColourSlot : public CommandSlider
	{
	private:
		const int slot;
		int pending_colour = -1;

	public:
		explicit CommandCeoColourSlot(CommandList* parent, int slot)
			: CommandSlider(parent, LIT(fmt::to_string(slot)), { CMDNAME("ceocolour") }, NOLABEL, 0, 0, 0, 1, CMDFLAGS_SLIDER & ~CMDFLAG_SUPPORTS_STATE_OPERATIONS), slot(slot)
		{
			if (slot != 0)
			{
				flags |= CMDFLAG_FEATURELIST_FINISHLIST;
			}
			command_names.at(0).append(utf8ToCmdName(fmt::to_string(slot)));
		}

		void onChange(Click& click, int prev_value) final
		{
			if (!click.isStand())
			{
				if (pending_colour == -1)
				{
					FiberPool::queueJob([this]
					{
						while (!ScriptHostUtil::canSafelyTakeIt())
						{
							Script::current()->yield();
						}
						if (ScriptHostUtil::takeItYielding())
						{
							ScriptGlobal(GLOBAL_CEO_COLOUR_SLOTS).at(slot, GLOBAL_CEO_COLOUR_SLOTS_SIZE).at(GLOBAL_CEO_COLOUR_SLOT_COLOUR).set<int>(pending_colour);
						}
						pending_colour = -1;
					});
				}
				pending_colour = this->value;
				ScriptGlobal(GLOBAL_CEO_COLOUR_SLOTS).at(slot, GLOBAL_CEO_COLOUR_SLOTS_SIZE).at(GLOBAL_CEO_COLOUR_SLOT_COLOUR).set<int>(this->value);
			}
		}

		void onTickInGameViewport() final
		{
			auto base = ScriptGlobal(GLOBAL_CEO_COLOUR_SLOTS).at(slot, GLOBAL_CEO_COLOUR_SLOTS_SIZE);
			auto boss = base.at(GLOBAL_CEO_COLOUR_SLOT_BOSS).get<int>();
			int colour = pending_colour;
			if (colour == -1)
			{
				colour = base.at(GLOBAL_CEO_COLOUR_SLOT_COLOUR).get<int>();
			}
			Click click{ CLICK_AUTO, TC_SCRIPT_NOYIELD, true };
			if (boss == -1 || colour == -1)
			{
				setHelpText(NOLABEL);
				setValue(click, 0);
				setMaxValue(0);
			}
			else
			{
				setHelpText(LIT(LANG_FMT("CEOCOL_H", AbstractPlayer(boss).getName())));
				setValue(click, colour);
				setMaxValue(14);
			}
		}

		void onTickInWebViewport() final
		{
			return onTickInWebViewportImplRedirect();
		}
	};
}
