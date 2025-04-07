#pragma once

#include "CommandAction.hpp"

#include "ScriptLocal.hpp"
#include "script_thread.hpp"

namespace Stand
{
	class CommandInteractionMenuClick : public CommandAction
	{
	private:
		const int submenu_id;

	public:
		explicit CommandInteractionMenuClick(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, int submenu_id)
			: CommandAction(parent, std::move(menu_name), std::move(command_names)), submenu_id(submenu_id)
		{
		}

	private:
		bool isClickResultInEffect() const noexcept
		{
			if (submenu_id == GLOBAL_AM_PI_MENU_SUBMENU_CEO)
			{
				if (g_player.isBoss() && g_player.isInMc() == false)
				{
					return true;
				}
			}
			else if (submenu_id == GLOBAL_AM_PI_MENU_SUBMENU_MC)
			{
				if (g_player.isBoss() && g_player.isInMc() == true)
				{
					return true;
				}
			}
			return false;
		}

	public:
		void onClick(Click& click) final
		{
			auto* thread = GtaThread::fromHash(ATSTRINGHASH("am_pi_menu"));
			if (thread == nullptr)
			{
				return click.setResponse(LOC("CMDONL"));
			}
			if (isClickResultInEffect())
			{
				return click.uwotm8();
			}
			ensureYieldableScriptThread(click, [this, thread]
			{
				uint8_t i = 0;
				do
				{
					ScriptGlobal(GLOBAL_AM_PI_MENU_SUBMENU).set(submenu_id);
					*ScriptLocal(thread, LOCAL_PI_STATE).as<int*>() = 1;
					*ScriptLocal(thread, LOCAL_PI_BITSET).as<int*>() |= (1 << 5);
					*ScriptLocal(thread, LOCAL_PI_BITSET).as<int*>() |= (1 << 11);
					Script::current()->yield();
					if (isClickResultInEffect())
					{
						break;
					}
					*ScriptLocal(thread, LOCAL_PI_STATE).as<int*>() = 0;
					Script::current()->yield();
				} while (!isClickResultInEffect() && i++ < 10);
			});
		}
	};
}
