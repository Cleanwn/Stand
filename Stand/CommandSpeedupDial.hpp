#pragma once

namespace Stand
{
	class CommandSpeedupDial : public CommandToggle
	{
	public:
		explicit CommandSpeedupDial(CommandList* const parent)
			: CommandToggle(parent, LOC("SPDUPDIAL"), CMDNAMES("speeddial", "speedial", "quickdial", "fastdial", "speedupdial"))
		{
		}

		void onChange(Click& click) final
		{
			onChangeToggleScriptTickEventHandler(click, []
			{
				*ScriptGlobal(GLOBAL_DIAL_TIME).as<int*>() = 0;
			});
			if (!m_on)
			{
				ensureScriptThread(click, []
				{
					*ScriptGlobal(GLOBAL_DIAL_TIME).as<int*>() = 5000;
				});
			}
		}
	};
}
