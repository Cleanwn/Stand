#pragma once

#include "CommandToggle.hpp"

#include "ScriptFuncPatch.hpp"

namespace Stand
{
	class CommandAnyVehRc : public CommandToggle
	{
	private:
		ScriptFuncPatch freemode_patch;
		ScriptFuncPatch am_mp_rc_vehicle_patch;

	public:
		inline static bool is_enabled = false;

		explicit CommandAnyVehRc(CommandList* const parent);
		
		void onEnable(Click& click) final;
	};
}
