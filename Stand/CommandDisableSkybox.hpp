#pragma once

#include "CommandToggle.hpp"

#include "pointers.hpp"

namespace Stand
{
	class CommandDisableSkybox : public CommandToggle
	{
	public:
		soup::Bytepatch skybox_patch{};

		explicit CommandDisableSkybox(CommandList* const parent)
			: CommandToggle(parent, LOC("NOSKY"), { CMDNAME("nosky") }, LOC("GEN_H_LOC"))
		{
		}

		void onEnable(Click& click) final
		{
			uint8_t patch[] = {
				soup::Bytepatch::RET
			};
			skybox_patch.initPatch(pointers::skybox_patch_ptr, patch, COUNT(patch));
		}

		void onDisable(Click& click) final
		{
			skybox_patch.restore();
		}
	};
}
