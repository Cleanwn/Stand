#pragma once

#include "fwddecl.hpp"
#include "typedecl.hpp"

#include <string>

namespace Stand
{
#pragma pack(push, 1)
	struct ToggleCorrelation
	{
		enum Type : ToggleCorrelation_t
		{
			NONE = 0,
			MENU_OPEN,
			ON_FOOT,
			AIMING,
			FREEROAM,
			CHATTING,
			SESSION_HOST,

			_NUM_TOGGLE_CORRELATIONS
		};

		Type type : 7 = NONE;
		bool invert : 1 = false;

		constexpr ToggleCorrelation() noexcept = default;

		constexpr ToggleCorrelation(Type type) noexcept
			: type(type)
		{
		}

		constexpr ToggleCorrelation(Type type, bool invert) noexcept
			: type(type), invert(invert)
		{
		}

		[[nodiscard]] static Label getCondition(ToggleCorrelation::Type type);
		[[nodiscard]] Label getCondition() const;

		[[nodiscard]] static bool getCurrentValue(ToggleCorrelation::Type correlation);
		[[nodiscard]] static bool getCurrentValue(ToggleCorrelation::Type correlation, bool correlation_invert);
		[[nodiscard]] bool getCurrentValue() const;

		[[nodiscard]] bool isActive() const;
		[[nodiscard]] std::string getState() const;
		[[nodiscard]] std::string getExplanation() const; // localised
	};
#pragma pack(pop)
}
