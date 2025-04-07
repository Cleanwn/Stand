#pragma once

#include "tbToggleableBehaviourScript.hpp"

namespace Stand
{
	class tbStareAtSky : public tbToggleableBehaviourScript
	{
	protected:
		void onTick() final;
		void onDisable() final;
	};

	inline tbStareAtSky g_tb_stare_at_sky{};
}
