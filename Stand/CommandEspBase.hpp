#pragma once

#include "AbstractPlayer.hpp"
#include "CommandColour.hpp"
#include "CommandEspTags.hpp"

namespace Stand
{
	template <class T>
	class CommandEspBase : public T
	{
	public:
		using T::T;

		CommandColour* colour = nullptr;
		CommandEspTags* tag_colours = nullptr;

	protected:
		[[nodiscard]] DirectX::SimpleMath::Color getColour(AbstractEntity& entity) const
		{
			return colour->getRGBA();
		}

		[[nodiscard]] DirectX::SimpleMath::Color getColour(AbstractPlayer& player) const
		{
			return tag_colours->getColour(player);
		}

		[[nodiscard]] DirectX::SimpleMath::Color getColour(AbstractPlayer&& player) const
		{
			return tag_colours->getColour(player);
		}
	};
}