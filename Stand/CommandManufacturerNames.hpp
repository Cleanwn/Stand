#pragma once

#include "CommandToggle.hpp"

#include "CommandVehicle.hpp"

namespace Stand
{
	class CommandManufacturerNames : public CommandToggle
	{
	private:
		CommandList* const dlcs;

	public:
		explicit CommandManufacturerNames(CommandList* const parent, CommandList* dlcs)
			: CommandToggle(parent, LOC("MANUFNS")), dlcs(dlcs)
		{
		}
		
		void onChange(Click& click) final
		{
			dlcs->recursiveForEachNonListChild([this](Command* cmd)
			{
				((CommandVehicle*)cmd)->updateName(true, m_on);
			});
		}
	};
}
