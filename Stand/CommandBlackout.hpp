#pragma once

#include "CommandListSelect.hpp"

#include "natives.hpp"

namespace Stand
{
	class CommandBlackout : public CommandListSelect
	{
	public:
		explicit CommandBlackout(CommandList* const parent)
			: CommandListSelect(parent, LOC("CTOS"), { CMDNAME("blackout") }, LOC("GEN_H_LOC"), {
				{0, LOC("DOFF")},
				{1, LOC("ON")},
				{2, LOC("CTOS_2")}
			}, 0)
		{
		}

	private:
		static void loop_1()
		{
			GRAPHICS::SET_ARTIFICIAL_LIGHTS_STATE(true);
			GRAPHICS::SET_ARTIFICIAL_VEHICLE_LIGHTS_STATE(false);
		}

		static void loop_2()
		{
			GRAPHICS::SET_ARTIFICIAL_LIGHTS_STATE(true);
			GRAPHICS::SET_ARTIFICIAL_VEHICLE_LIGHTS_STATE(true);
		}

	public:
		void onChange(Click& click, long long prev_value) final
		{
			onChangeProcessScriptTickEventHandler(click, prev_value, 1, &loop_1);
			onChangeProcessScriptTickEventHandler(click, prev_value, 2, &loop_2);
			if (value == 0)
			{
				ensureScriptThread(click, []
				{
					GRAPHICS::SET_ARTIFICIAL_LIGHTS_STATE(false);
				});
			}
		}
	};
}
