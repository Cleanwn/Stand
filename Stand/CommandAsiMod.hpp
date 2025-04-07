#pragma once

#include "CommandToggle.hpp"

#include "AsiMod.hpp"
#include "Tunables.hpp"

namespace Stand
{
	class CommandAsiMod : public CommandToggle
	{
	public:
		std::unique_ptr<AsiMod> asi;

		explicit CommandAsiMod(CommandList* const parent, Label&& menu_name, const AsiDirectory dir)
			: CommandToggle(parent, std::move(menu_name), {}, NOLABEL, false, CMDFLAGS_TOGGLE | CMDFLAG_SEARCH_FINISHLIST | CMDFLAG_FEATURELIST_SKIP), asi(std::make_unique<AsiMod>(this, dir))
		{
		}

		void onEnable(Click& click) final
		{
			if (g_tunables.getBool(TUNAHASH("DISABLE_ASIS")))
			{
				click.setResponse(LOC("CMDDISA"));
				setOnIndication(false);
				return;
			}
			click.stopInputIfAllowed();
			ensureScriptThread(click, [this]
			{
				asi->load();
			});
		}

		void onDisable(Click& click) final
		{
			click.stopInputIfAllowed();
			bool notify = !click.isAuto();
			ensureScriptThread(click, [this, notify]
			{
				asi->unload(notify);
			});
		}
	};
}
