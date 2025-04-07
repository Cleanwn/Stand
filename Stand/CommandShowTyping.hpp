#pragma once

#include "CommandToggle.hpp"

#include "Position2d.hpp"

#undef NONE

namespace Stand
{
	class CommandShowTyping : public CommandToggle
	{
	public:
		enum Preview : uint8_t
		{
			NONE,
			CLOSED,
			OPEN,
		};

		inline static CommandShowTyping* instance;
		bool enabled_for_preview = false;
		inline static Preview preview = NONE;
		inline static Position2d pos_closed = { 1217, 1080 };
		inline static Position2d pos_open = { 1217, 1025 };

		explicit CommandShowTyping(CommandList* const parent);

		void onEnable(Click&) final;
		void onDisable(Click&) final;
	private:
		static void presentCallback();
	};
}
