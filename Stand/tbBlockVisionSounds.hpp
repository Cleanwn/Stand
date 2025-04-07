#pragma once

#include "tbToggleableBehaviour.hpp"

namespace Stand
{
	class tbBlockVisionSounds : public tbToggleableBehaviour
	{
	protected:
		void onEnable() final;
		void onDisable() final;
	};

	inline tbBlockVisionSounds g_tb_block_vision_sounds{};
}
