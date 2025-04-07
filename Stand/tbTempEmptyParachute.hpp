#pragma once

#include "tbToggleableBehaviour.hpp"

namespace Stand
{
	class tbTempEmptyParachute : public tbToggleableBehaviour
	{
	private:
		bool had_parachute;
		bool had_reserve_parachute;

	protected:
		void onEnable() final;
		void onDisable() final;
	};

	inline tbTempEmptyParachute g_tb_temp_empty_parachute{};
}
