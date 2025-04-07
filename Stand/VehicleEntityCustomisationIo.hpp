#pragma once

#include "VehicleCustomisationIo.hpp"

#include "AbstractEntity.hpp"

namespace Stand
{
	class VehicleEntityCustomisationIo : public VehicleCustomisationIo
	{
	public:
		AbstractEntity veh;

		VehicleEntityCustomisationIo(AbstractEntity veh)
			: veh(std::move(veh))
		{
		}

		[[nodiscard]] Mixed read(int idx) final;
		void write(int idx, Mixed&& value) final;
	};
}
