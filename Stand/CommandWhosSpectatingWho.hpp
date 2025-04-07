#pragma once

#include "CommandToggle.hpp"

#include "fmt_arg.hpp"

namespace Stand
{
	class CommandWhosSpectatingWho : public CommandToggle
	{
	public:
		explicit CommandWhosSpectatingWho(CommandList* const parent)
			: CommandToggle(parent, LOC("WSPCW"), CMDNAMES_OBF("infowhospectateswho"))
		{
		}

		void onChange(Click& click) final
		{
			onChangeToggleScriptTickEventHandler(click, []
			{
				for (const auto& p : AbstractPlayer::listExcept(true))
				{
					if (p.getPed().getHealth() != 0.0f) // ignore kill cam
					{
						if (auto spectatee = p.getSpectateTarget(); spectatee.isValid())
						{
							drawDebugText(LANG_FMT("WSPCW_T", FMT_ARG("spectator", p.getName()), FMT_ARG("spectatee", spectatee.getName())));
						}
					}
				}
			});
		}
	};
}
