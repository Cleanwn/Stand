#pragma once

#include "CommandAction.hpp"

namespace Stand
{
	class CommandPlayerClassifierDetection : public CommandAction
	{
	public:
		const flowevent_t event_type;
		bool preemptive = false;

		explicit CommandPlayerClassifierDetection(CommandList* const parent, const flowevent_t event_type, Label&& name, uint8_t severity);

		void updateSeverity(uint8_t severity);

		void onClick(Click& click) final;

		void onDetectionRemoved(ThreadContext tc);
	};
}
