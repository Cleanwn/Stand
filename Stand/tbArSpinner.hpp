#pragma once

#include "tbToggleableBehaviourScript.hpp"

namespace Stand
{
	class tbArSpinner : public tbToggleableBehaviourScript
	{
	private:
		float line_i = 360.0f;

	protected:
		void onTick() final;
	};

	inline tbArSpinner g_tb_ar_spinner{};
};
