#pragma once

#include "CommandList.hpp"

namespace Stand
{
	class CommandListNonDefaults : public CommandList
	{
	private:
		bool active = false;

	public:
		explicit CommandListNonDefaults(CommandList* parent);

		void onActiveListUpdate() final;
	};
}
