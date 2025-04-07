#pragma once

#include "CommandToggle.hpp"

#include "pointers.hpp"

namespace Stand
{
	class CommandFlyThroughWindscreen : public CommandToggle
	{
	private:
		soup::Bytepatch patch;

	public:
		static inline CommandToggle* instance = nullptr;

		explicit CommandFlyThroughWindscreen(CommandList* const parent)
			: CommandToggle(parent, LOC("FLYTHRUWND"), CMDNAMES_OBF("flythroughwindscreen"), LOC("FLYTHRUWND_H"))
		{
			instance = this;
		}

		void onEnable(Click& click) final
		{
			patch.initPatchNOP(pointers::CTaskMotionInVehicle_CheckForThroughWindow_patch, 7);
		}

		void onDisable(Click& click) final
		{
			patch.restore();
		}
	};
}
