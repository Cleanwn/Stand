#pragma once

#include "CustomisationIo.hpp"

#include "fwddecl.hpp"
#include "natives_decl.hpp"

#include "VehicleMods.hpp"

namespace Stand
{
	class VehicleCustomisationIo : public CustomisationIo
	{
	public:
		enum Properties : int
		{
			MODEL = 0,
			PLATE_STYLE,
			PLATE_TEXT,
			PRIMARY_COLOUR,
			SECONDARY_COLOUR,
			PEARL_COLOUR,
			WHEEL_COLOUR,
			WHEEL_TYPE,
			_MODS_BEGIN,
			_MODS_END = _MODS_BEGIN + VehicleMods::_NUM_TYPES - 1,
			TYRE_SMOKE_COLOUR,
			WINDOW_TINT,
			CUSTOM_PRIMARY_COLOUR,
			CUSTOM_SECONDARY_COLOUR,
			NEON_COLOUR,
			NEON_FRONT,
			NEON_BACK,
			NEON_LEFT,
			NEON_RIGHT,
			ENVEFF_SCALE,
			INTERIOR_COLOUR,
			ROOF_LIVERY,
			DASHBOARD_COLOUR,
			TYRES_MODE,
			EXTRA_1,
			EXTRA_2,
			EXTRA_3,
			EXTRA_4,
			EXTRA_5,
			EXTRA_6,
			EXTRA_7,
			EXTRA_8,
			EXTRA_9,
			EXTRA_10,
			EXTRA_11,
			EXTRA_12,
			EXTRA_13,
			EXTRA_14,
			EXTRA_15,

			_NUM_PROPERIES
		};

		[[nodiscard]] int getNumProperties() const noexcept final
		{
			return _NUM_PROPERIES;
		}

		[[nodiscard]] static constexpr bool isVehicleMod(int idx)
		{
			return idx >= _MODS_BEGIN && idx <= _MODS_END;
		}

		[[nodiscard]] static constexpr int translatePropertyToVehicleMod(int idx)
		{
			return idx - _MODS_BEGIN;
		}

		[[nodiscard]] static bool isPropertyApplicable(AbstractEntity& veh, int idx);

		void applyTo(AbstractEntity& veh);
		[[nodiscard]] AbstractEntity createEntityNow(bool networked);
		void spawnClone(Click& click);
	};
}
