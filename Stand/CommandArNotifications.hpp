#pragma once

#include "CommandToggle.hpp"

#include "Scaleform.hpp"

namespace Stand
{
	class CommandArNotifications : public CommandToggle
	{
	private:
		Scaleform scaleform = "game_stream";
	public:
		CommandToggleNoCorrelation* debug_mode;
		CommandSliderFloat* dynamic_scaler;
		CommandSliderFloat* static_scaler;
		CommandToggleNoCorrelation* dynamic_scaling;
		CommandListSelect* rotation_mode;
		CommandSliderFloat* horizontal_offset;
		CommandSliderFloat* vertical_offset;
		CommandSliderFloat* depth_offset;
		CommandSliderFloat* fp_horizontal_offset;
		CommandSliderFloat* fp_vertical_offset;
		CommandSliderFloat* fp_depth_offset;
		CommandSliderFloat* top_offset;
		CommandToggleNoCorrelation* ensure_text_readability;

		explicit CommandArNotifications(CommandList* const parent);

		void onEnable(Click& click) final;
		void onDisable(Click& click) final;
	protected:
		void ensureTextReadability(v3& notif_rot) const;
		void free();
	};
}
