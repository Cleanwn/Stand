#pragma once

#include "CommandList.hpp"

namespace Stand
{
	class CommandPlayerClassifier : public CommandList
	{
	public:
		explicit CommandPlayerClassifier(CommandList* const parent);

		void onClick(Click& click) final;

		[[nodiscard]] CommandPlayerClassifierDetection* getDetection(const flowevent_t type) const;
	};
}
