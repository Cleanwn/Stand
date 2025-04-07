#pragma once

#include "CommandStatSlider.hpp"

#include <soup/time.hpp>

#include "Date.hpp"
#include "Offset.hpp"

namespace Stand
{
	enum DateType : uint8_t
	{
		YEAR,
		MONTH,
		DAY,
		HOUR,
		MINUTE,
		SECOND,
		MILLISECOND
	};

	class CommandStatDateField : public CommandStatSlider<CommandSlider>
	{
	private:
		DateType type;
		Offset<Date, rage::scrValue> offset;

		[[nodiscard]] static int getMinValue(DateType type) noexcept
		{
			switch (type)
			{
			case YEAR:
				return 2013;

			case HOUR:
				return 0;

			default:
				return 1;
			}
		}

		[[nodiscard]] static int getMaxValue(DateType type)
		{
			switch (type)
			{
			case YEAR:
				return soup::time::datetimeLocal(soup::time::unixSeconds()).year;

			case MONTH:
				return 12;
				
			case DAY:
				return 31; // Consider shorter months.

			case HOUR:
				return 23;

			case MINUTE:
			case SECOND:
				return 60;

			case MILLISECOND:
				return 999;
			}

			SOUP_ASSERT(false);
		}

	public:
		explicit CommandStatDateField(CommandList* const parent, Label&& name, DateType type, Offset<Date, rage::scrValue> offset, Stat& stat)
			: CommandStatSlider(parent, stat, getMinValue(type), getMaxValue(type)), type(type), offset(offset)
		{
			setMenuName(std::move(name));
			addSuffixToCommandNames(this->menu_name.getEnglishForCommandName());
		}

	protected:
		void setStat(Hash stat) final
		{
			Date date { };
			STATS::STAT_GET_DATE(stat, (Any*)&date, 7, TRUE);
			offset.resolve(date).Int = this->value;
			STATS::STAT_SET_DATE(stat, (Any*)&date, 7, TRUE);
		}

		void getStat(Hash stat, int* value) final
		{
			Date date { };
			STATS::STAT_GET_DATE(stat, (Any*)&date, 7, TRUE);
			*value = offset.resolve(date).Int;
		}
	};
}