#pragma once

#include "CommandListViewport.hpp"
#include "CommandListConcealer.hpp"

namespace Stand
{
	class CommandListWeaponComponents : public CommandListViewport<CommandListConcealer>
	{
	public:
		hash_t weapon = 'DEEZ';

		explicit CommandListWeaponComponents(CommandList* const parent);

		void onTickAsActiveList() final;
	};
}
