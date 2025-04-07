#pragma once

#include "CommandListSelect.hpp"

namespace Stand
{
	class CommandPersonalVehiclesSort : public CommandListSelect
	{
	public:
		explicit CommandPersonalVehiclesSort(CommandList* parent)
			: CommandListSelect(parent, LOC("SRTBY"), {}, NOLABEL, {
				{ reinterpret_cast<long long>(&CommandPersonalVehicles::sortAlgoName), LOC("NME") },
				{ reinterpret_cast<long long>(&CommandPersonalVehicles::sortAlgoSlot), LOC("SRTBY_SLT") },
			}, reinterpret_cast<long long>(&CommandPersonalVehicles::sortAlgoName))
		{
		}

		void onChange(Click& click, long long prev_value) final
		{
			((CommandListRefreshable*)parent)->sort();
		}
	};
}
