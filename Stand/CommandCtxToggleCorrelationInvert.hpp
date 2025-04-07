#pragma once

#include "CommandToggleNoCorrelation.hpp"

namespace Stand
{
	class CommandCtxToggleCorrelationInvert : public CommandToggleNoCorrelation
	{
	public:
		CommandCtxToggleCorrelation* const autostate;

		explicit CommandCtxToggleCorrelationInvert(CommandList* const parent, CommandToggle* const target, CommandCtxToggleCorrelation* autostate)
			: CommandToggleNoCorrelation(parent, LOC("AUTOSTATEINV"), {}, NOLABEL, target->correlation.invert), autostate(autostate)
		{
			autostate->invert = this;
			autostate->updateHelpTexts(target);
		}

		void onChange(Click& click) final
		{
			auto* const toggle = (CommandToggle*)ContextMenu::getTargetPhysical();
			toggle->correlation.invert = m_on;
			auto click_ = click.deriveAuto();
			toggle->updateState(click_);
			toggle->processCorrelationUpdate(click_);
			toggle->setStateBasedOnCorrelation(click_);
		}
	};
}
