#pragma once

#include "tbToggleableBehaviourScript.hpp"

namespace Stand
{
	class tbDeceiveInteriorChecks : public tbToggleableBehaviourScript
	{
	protected:
		void onEnable() final;
		void onDisable() final;
		void onTick() final;
	};

	inline tbDeceiveInteriorChecks g_tb_deceive_interior_checks{};
}
