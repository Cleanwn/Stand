#pragma once

#include "tbToggleableBehaviourScript.hpp"

namespace Stand
{
	class tbScreenshotMode : public tbToggleableBehaviourScript
	{
	public:
		bool allow_centred_text = false;

	protected:
		void onEnable() override;
		void onDisable() final;
		void onTick() final;
	};

	inline tbScreenshotMode g_tb_screenshot_mode{};
}
