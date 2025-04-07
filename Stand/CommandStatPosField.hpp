#pragma once

#include "Stats.hpp"
#include "CommandStatSlider.hpp"

namespace Stand
{
	enum PosType
	{
		X,
		Y,
		Z,
	};

	class CommandStatPosField : public CommandStatSlider<CommandSliderFloat>
	{
	private:
		PosType type;

		[[nodiscard]] static std::string getLiteralName(PosType type) noexcept
		{
			switch (type)
			{
			case X:
				return "X";

			case Y:
				return "Y";

			case Z:
				return "Z";
			}

			SOUP_ASSERT(false);
		}

	public:
		explicit CommandStatPosField(CommandList* const parent, PosType type, Stat& stat)
			: CommandStatSlider(parent, stat, -INT32_MAX, INT32_MAX), type(type)
		{
			std::string name = getLiteralName(type);
			setMenuName(LIT(name));
			addSuffixToCommandNames(utf8ToCmdName(std::move(name)));
		}

	protected:
		void setStat(Hash stat) final
		{
			float x, y, z;
			STATS::STAT_GET_POS(stat, &x, &y, &z, -1);

			switch (type)
			{
			case X:
				x = getFloatValue();
				break;

			case Y:
				y = getFloatValue();
				break;

			case Z:
				z = getFloatValue();
				break;
			}

			STATS::STAT_SET_POS(stat, x, y, z, TRUE);
		}

		void getStat(Hash stat, int* value) final
		{
			float r = 0.0f;
			float x, y, z;
			STATS::STAT_GET_POS(stat, &x, &y, &z, -1);

			switch (type)
			{
			case X:
				r = x;
				break;

			case Y:
				r = y;
				break;

			case Z:
				r = z;
				break;
			}

			*value = (int)(getPrecisionScalar() * r);
		}
	};
}