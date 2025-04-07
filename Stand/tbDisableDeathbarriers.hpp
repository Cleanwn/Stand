#pragma once

#include "tbToggleableBehaviour.hpp"

namespace Stand
{
	class tbDisableDeathbarriers : public tbToggleableBehaviour
	{
	protected:
		void onEnable() final;
	};

	inline auto g_tb_disable_deathbarriers = tbDisableDeathbarriers();
}
