#include "CommandCtxToggleCorrelation.hpp"

#include "CommandToggle.hpp"
#include "ContextMenu.hpp"

namespace Stand
{
	CommandCtxToggleCorrelation::CommandCtxToggleCorrelation(CommandList* const parent, CommandToggle* const target)
		: CommandListSelect(parent, LOC("AUTOSTATE"), {}, NOLABEL, {
			{(long long)ToggleCorrelation::NONE, LOC("AUTOSTATE_OFF")},
			{(long long)ToggleCorrelation::MENU_OPEN, LOC("AUTOSTATE_OPN")},
			{(long long)ToggleCorrelation::ON_FOOT, LOC("AUTOSTATE_OFT")},
			{(long long)ToggleCorrelation::AIMING, LOC("AUTOSTATE_AIM")},
			{(long long)ToggleCorrelation::FREEROAM, LOC("AUTOSTATE_FRE")},
			{(long long)ToggleCorrelation::CHATTING, LOC("AUTOSTATE_CHT")},
			{(long long)ToggleCorrelation::SESSION_HOST, LOC("AUTOSTATE_HST")},
		}, (long long)target->correlation.type)
	{
	}

	void CommandCtxToggleCorrelation::updateHelpTexts(const CommandToggle* const toggle)
	{
		Label explanation = LIT(toggle->correlation.getExplanation());
		setHelpText(explanation);
		invert->setHelpText(std::move(explanation));
	}

	void CommandCtxToggleCorrelation::onChange(Click& click, long long prev_value)
	{
		auto* const toggle = (CommandToggle*)ContextMenu::getTargetPhysical();
		toggle->correlation.type = (ToggleCorrelation::Type)value;
		auto click_ = click.deriveAuto();
		toggle->updateState(click_);
		toggle->processCorrelationUpdate(click_);
		toggle->setStateBasedOnCorrelation(click_);
	}
}
