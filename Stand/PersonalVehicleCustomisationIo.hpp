#pragma once

#include "VehicleCustomisationIo.hpp"

#include "PersonalVehicle.hpp"

namespace Stand
{
	class PersonalVehicleCustomisationIo : public VehicleCustomisationIo
	{
	public:
		PersonalVehicle pv;

		explicit PersonalVehicleCustomisationIo(const uint32_t slot)
			: pv{ slot }
		{
		}
		
		explicit PersonalVehicleCustomisationIo(PersonalVehicle&& pv)
			: pv(std::move(pv))
		{
		}

		[[nodiscard]] Mixed read(int idx) final;
		void write(int idx, Mixed&& value) final;
	private:
		void writeCustomColourStats(ScriptGlobal base) const;
		[[nodiscard]] int getPackedIntIndex(const int iParam0) const;
		[[nodiscard]] int getPackedIntHighModTypeIndex() const;
		[[nodiscard]] int getPackedIntInteriorColourIndex() const;
		[[nodiscard]] int getPackedIntRoofLiveryIndex() const;
		[[nodiscard]] int getPackedIntDashboardColourIndex() const;
		[[nodiscard]] int getPackedIntTyresModeIndex() const;

		[[nodiscard]] static int extraToFlagsIndex(const int iParam0);
	};
}
