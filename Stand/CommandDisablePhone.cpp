#include "CommandDisablePhone.hpp"

#include "get_current_time_millis.hpp"
#include "gta_input.hpp"
#include "Gui.hpp"
#include "natives.hpp"
#include "ScriptGlobal.hpp"

namespace Stand
{
	CommandDisablePhone::CommandDisablePhone(CommandList* const parent)
		: CommandToggle(parent, LOC("DSBLPHNE"), combineCommandNames(CMDNAMES("no", "block", "disable"), CMDNAME("phonespam")), LOC("DSBLPHNE_H"))
	{
		this->command_names.emplace_back(CMDNAME("straighttovoicemail"));
		this->command_names.emplace_back(CMDNAME("straightovoicemail"));
	}

	void CommandDisablePhone::onEnable(Click& click)
	{
		g_gui.phone_disabled = true;
		registerScriptTickEventHandler(click, []
		{
			HANDLER_CHECK(g_gui.phone_disabled);
			if (g_gui.isPhoneClosed())
			{
				g_gui.phone_stay_closed = true;
				if (*ScriptGlobal(GLOBAL_INPUT_PHONE).as<int*>() == INPUT_PHONE && PAD::IS_DISABLED_CONTROL_JUST_PRESSED(0, INPUT_PHONE))
				{
					g_gui.phone_open_deadline = get_current_time_millis() + 300;
				}
				*ScriptGlobal(GLOBAL_DISABLE_PHONE).as<BOOL*>() = get_current_time_millis() > g_gui.phone_open_deadline;
			}
			else
			{
				if (g_gui.phone_open_deadline >= get_current_time_millis())
				{
					g_gui.phone_stay_closed = false;
				}
				*ScriptGlobal(GLOBAL_DISABLE_PHONE).as<BOOL*>() = g_gui.phone_stay_closed;
			}
			HANDLER_END;
		});
	}

	void CommandDisablePhone::onDisable(Click& click)
	{
		g_gui.phone_disabled = false;
		ensureScriptThread(click, []
		{
			*ScriptGlobal(GLOBAL_DISABLE_PHONE).as<BOOL*>() = false;
		});
	}
}
