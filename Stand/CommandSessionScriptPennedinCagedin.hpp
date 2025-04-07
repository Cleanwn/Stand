#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandSessionScriptPennedinCagedin : public CommandToggle
	{
	public:
		explicit CommandSessionScriptPennedinCagedin(CommandList* const parent)
			: CommandToggle(parent, LOC("SCR_FE_PI_CI"), {}, NOLABEL, false, CMDFLAGS_TOGGLE | CMDFLAG_TEMPORARY)
		{
		}

		void onChange(Click& click) final
		{
			if (!click.isUltimateEdition())
			{
				return onChangeImplUnavailable(click);
			}
			const bool val = m_on;
			click.ensureYieldableScriptThread([this, val]
			{
				if (auto thread = parent->as<CommandSessionScript>()->getThreadAndEnsureControl())
				{
					*ScriptLocal(thread, LOCAL_PNDIN_SHDATA + 197).as<int*>() = (val ? 2 : 0);
				}
			});
		}

		void onTickInGameViewport() final
		{
			if (auto thread = parent->as<CommandSessionScript>()->getThread())
			{
				setOnIndication(*ScriptLocal(thread, LOCAL_PNDIN_SHDATA + 197).as<int*>() == 2);
			}
		}

		void onTickInWebViewport() final
		{
			return onTickInWebViewportImplRedirect();
		}
	};
}
