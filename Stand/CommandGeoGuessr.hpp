#pragma once

#include "CommandListConcealer.hpp"

#include "vector.hpp"

namespace Stand
{
	class CommandGeoGuessr : public CommandListConcealer
	{
	public:
		bool scouting = false;
		time_t guessed_at = 0;
		v3 guess;

		CommandToggleNoCorrelation* scout_toggle;
		CommandAction* submit_action;

		explicit CommandGeoGuessr(CommandList* const parent);

		void startScouting();
		void stopScouting();

		void updateGameCommandsVisiblity(const bool toggle);

		void cleanup();
		void cleanupCam();
		void cleanupGuess();
	};
}
