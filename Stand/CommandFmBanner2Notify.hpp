#pragma once

#include "CommandToggle.hpp"

#include "FmBanner.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandFmBanner2Notify : public CommandToggle
	{
	public:
		explicit CommandFmBanner2Notify(CommandList* const parent)
			: CommandToggle(parent, LOC("FMBNR2T"), CMDNAMES("fmbanner2notify", "fmbannertonotifications"))
		{
		}

		void onChange(Click& click) final
		{
			onChangeToggleScriptTickEventHandler(click, []
			{
				if (is_session_started()
					&& FmBanner::isSlotInUse(0)
					)
				{
					auto b = FmBanner::fromSlot(0);
					//g_logger.log(fmt::format("fm banner: {}, {}, {}", script_global(GLOBAL_FMBNR_BASE).at(0, GLOBAL_FMBNR_SIZE).at(GLOBAL_FMBNR_STYLE).get<int>(), b.headline, b.subtext));
					std::string text = Util::GET_LABEL_TEXT(b.headline);
					if (text != "NULL")
					{
						if (auto subtext = b.getSubtextString(); !subtext.empty() && subtext != "NULL")
						{
							text.append(": ");
							text.append(subtext);
						}
						Util::toast(std::move(text));

						ScriptGlobal(GLOBAL_FMBNR_BASE).at(0, GLOBAL_FMBNR_SIZE).at(GLOBAL_FMBNR_STATE).set<int>(5);
					}
				}
			});
		}
	};
}
