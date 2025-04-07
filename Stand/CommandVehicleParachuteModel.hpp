#pragma once

#include "CommandListSelect.hpp"

namespace Stand
{
	class CommandVehicleParachuteModel : public CommandListSelect
	{
	public:
		explicit CommandVehicleParachuteModel(CommandList* parent);

		void onChange(Click& click, long long prev_value) final;
		void apply() const;
	};
}
