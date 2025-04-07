#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandDiscord : public CommandToggle
	{
	public:
		inline static CommandDiscord* instance;

		explicit CommandDiscord(CommandList* const parent);

		void onEnable(Click& click) final;
		void onDisable(Click& click) final;

		void update() const;
		static void updatePresence();
	};
}
