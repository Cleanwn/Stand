#pragma once

#include "CommandToggleNoCorrelation.hpp"

#include "Gui.hpp"

namespace Stand
{
	class CommandProfileAutoload : public CommandToggleNoCorrelation
	{
	public:
		explicit CommandProfileAutoload(CommandList* const parent)
			: CommandToggleNoCorrelation(parent, LOC("AUTOLOAD"), {}, NOLABEL, false, CMDFLAGS_TOGGLE_NO_CORRELATION & ~CMDFLAG_SUPPORTS_STATE_OPERATIONS)
		{
			addSuffixToCommandNames(parent->menu_name.getLiteralForCommandName());
		}

		void onEnable(Click& click) final
		{
			g_gui.meta_state.data.at("Load On Inject") = parent->as<CommandProfile>()->getProfileNameA();
			g_gui.meta_state.save();
		}

		void onDisable(Click& click) final
		{
			g_gui.meta_state.data.at("Load On Inject").clear();
			g_gui.meta_state.save();
		}

		void onTickInGameViewport() final
		{
			setOnIndication(g_gui.meta_state.data.at("Load On Inject") == parent->as<CommandProfile>()->getProfileNameA());
		}

		void onTickInWebViewport() final
		{
			return onTickInWebViewportImplRedirect();
		}
	};
}
