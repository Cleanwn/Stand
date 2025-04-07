#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandCustomVoteKickToggle : public CommandToggle
	{
	public:
		ToastConfig toast_config{ true };
		PlayerExcludes exclusions;
		CommandSlider* threshold_slider;

		explicit CommandCustomVoteKickToggle(CommandList* const parent)
			: CommandToggle(parent, LOC("CSTMVK"), CMDNAMES_OBF("customvotekick"))
		{
		}

		void onEnable(Click& click) final
		{
			registerScriptTickEventHandler([this]
			{
				HANDLER_CHECK(this->m_on);
				for (const auto& p : AbstractPlayer::listExcept(true))
				{
					SOUP_IF_LIKELY (auto cmd = p.getCommand())
					{
						if (!exclusions.isExcluded(p))
						{
							if (!cmd->did_custom_vote_kick // Prevent flooding kick events if they end up failing. Makes us look less stupid.
								&& cmd->vote_kick_history.size() >= threshold_slider->value
								)
							{
								Util::toast(LANG_FMT("CSTMVK_MSG", p.getName()), toast_config.getToastFlags());
								cmd->did_custom_vote_kick = true;
								p.kick();
							}
						}
						else
						{
							// Avoid insta-kicking exclusions when we toggle them.
							// Or another example, excluding an org member and the org suddenly disbands.
							// Kind of a controversial change, but I figure "less is more" when it comes to kicking people.
							// Manually tracking vote kicks may appear redundant, but the ability to easily set this value to zero makes the aforementioned problem very simple to solve.
							// It also gives us the ability to track vote kicks of people who leave (or get forcefully removed). People often leave after vote-kicking since they're getting griefed. This can help ensure medium-to-high thresholds are still respected.
							cmd->vote_kick_history.clear();
						}
					}
				}
				HANDLER_END;
			});
		}
	};
}
