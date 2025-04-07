#pragma once

#include "Textbox.hpp"

namespace Stand
{
#pragma pack(push, 1)
	class Commandbox : public Textbox
	{
	public:
		bool active = false;
		bool enter_released = true;
		bool escape_released = true;
		[[nodiscard]] bool shouldBlockGameInputs() const noexcept;

		bool colour_selector_active = false;
		uint8_t colour_selector_cursor = 0;
		uint8_t colour_selector_cursor_pre_transpose = 0;
		[[nodiscard]] bool isColourSelectorActive() const noexcept;

		time_t last_command_issuance = 0;
		bool opened_for_chaining = false;

		void resetSecondaryStates() final;

	protected:
		void onUpdate() final;
		void onLeft() final;
		void onRight() final;
		void onUp() final;
		void onDown() final;
		void onEnter() final;
		void onEscape() final;
		void onCtrlU() final;

	private:
		static uint8_t transposeCursorDown(uint8_t cursor);
		void toggleColourSelectorRow();
	};
#pragma pack(pop)

	inline Commandbox g_commandbox = {};
}
