#pragma once

#include "Label.hpp"

namespace Stand
{
#pragma pack(push, 1)
	struct EventLabel
	{
		Label raw_name{};
		Label name{};

		constexpr EventLabel() noexcept = default;

		constexpr EventLabel(Label&& raw_name) noexcept
			: raw_name(std::move(raw_name)), name(Label(this->raw_name))
		{
		}

		constexpr EventLabel(Label&& raw_name, Label&& name) noexcept
			: raw_name(std::move(raw_name)), name(std::move(name))
		{
		}

		EventLabel(EventLabel&& b);
		
		void operator =(EventLabel&& label);

		void operator =(Label&& label);

		[[nodiscard]] std::string getName() const;

		[[nodiscard]] bool hasNonRawName() const;
	};
#pragma pack(pop)
}
