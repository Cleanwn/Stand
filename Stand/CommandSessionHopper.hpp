#pragma once

#include "CommandToggle.hpp"

#include "CommandSlider.hpp"

namespace Stand
{
	class CommandSessionHopper : public CommandToggle
	{
	public:
		inline static CommandSessionHopper* instance;

	private:
		CommandSlider* const min_players;
		bool job_in_progress = false;

	public:
		explicit CommandSessionHopper(CommandList* parent, CommandSlider* min_players);

		[[nodiscard]] bool shouldHop() const;

		void onEnable(Click& click) final;
	};
}
