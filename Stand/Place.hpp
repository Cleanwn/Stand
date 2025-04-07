#pragma once

#include "Vector2Plus.hpp"
#include "Label.hpp"

namespace Stand
{
	class PointOfPresence
	{
	public:
		Vehicle veh = INVALID_GUID;
		Blip blip = INVALID_BLIP_ID;
		Vector2Plus pos;
	};

	class Place
	{
	public:
		const Label name;

		explicit constexpr Place(Label&& name)
			: name(std::move(name))
		{
		}

		[[nodiscard]] virtual bool supportsUsageByOthers() const noexcept
		{
			return false;
		}

		// If returning std::nullopt, must use click.setResponse to inform user about why no PoP is available.
		// If returning PointOfPresence, click must be ignored.
		[[nodiscard]] virtual std::optional<PointOfPresence> getClosestPop(Click& click) = 0;
	};
}
