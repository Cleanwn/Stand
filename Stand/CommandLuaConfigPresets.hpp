#pragma once

#include "CommandListAction.hpp"

#include "CommandListActionItem.hpp"

namespace Stand
{
	class CommandLuaConfigPresets : public CommandListAction
	{
	public:
		explicit CommandLuaConfigPresets(CommandList* const parent)
			: CommandListAction(parent, LOC("PRSTS"), {}, NOLABEL, {
				{0, LOC("LUACFGPRST_0")},
				//{1, LOC("LUACFGPRST_1")},
				{2, LOC("LUACFGPRST_2")},
			})
		{
		}

		void onItemClick(Click& _click, CommandListActionItem* item) final
		{
			CommandListAction::onItemClick(_click, item);

			auto click = _click.deriveAuto();
			const auto value = item->value;

			LuaConfig::warn_money->setStateBool(click, value != 2);
			LuaConfig::warn_dangerous->setStateBool(click, value != 2);

			LuaConfig::enforce_silent_start->setValue(click, value == 2 ? 2 : 1);
			LuaConfig::enforce_silent_stop->setValue(click, value == 2 ? 2 : 1);

			LuaConfig::enable_warnings->setStateBool(click, value == 2);
			LuaConfig::notify_deprecated->setStateBool(click, value == 2);
			LuaConfig::notify_bad_practice->setStateBool(click, value == 2);
			LuaConfig::notify_missed_cleanup_blurrect->setStateBool(click, value == 2);
			LuaConfig::notify_missed_cleanup_arspinner->setStateBool(click, value == 2);
			LuaConfig::notify_missed_cleanup_graceland->setStateBool(click, value == 2);
			LuaConfig::notify_ref_by_path_upgrade->setStateBool(click, value == 2);
			LuaConfig::notify_get_value_fail->setStateBool(click, value == 2);
		}
	};
}
