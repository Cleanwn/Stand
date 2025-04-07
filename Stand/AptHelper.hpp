#pragma once

#include "vector.hpp"

namespace Stand
{
	struct AptHelper
	{
		static constexpr uint8_t max_pos_id = 83;
		[[nodiscard]] static v3 getPosFromId(const uint8_t pos_id);
		[[nodiscard]] static uint8_t getNearestPosId(const v3& target);
		[[nodiscard]] static v3 getNearestPos(const v3& target);

		static constexpr uint8_t max_property_id = 114;
		[[nodiscard]] static uint8_t propertyIdToPosId(const uint8_t property_id);
		[[nodiscard]] static uint8_t posIdToPropertyId(const uint8_t pos_id);
		[[nodiscard]] static std::string propertyIdToName(const uint8_t property_id);
	};
}
