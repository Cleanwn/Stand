#pragma once

#include "tbToggleableBehaviour.hpp"

namespace Stand
{
	class tbReticle : public tbToggleableBehaviour
	{
	private:
		static void onPresent();
	protected:
		void onEnable() final;
		void onDisable() final;
	};

	inline tbReticle g_tb_reticle{};
}
