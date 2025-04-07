#include "CommandStatDate.hpp"

#include "CommandStatDateField.hpp"
#include "Date.hpp"

namespace Stand
{
	CommandStatDate::CommandStatDate(CommandList* const parent, Stat& stat)
		: CommandList(parent, stat.getLabel(), {}, NOLABEL), stat(stat)
	{
		createChild<CommandStatDateField>(LOC("STAT_YEAR"), YEAR, &Date::year, stat);
		createChild<CommandStatDateField>(LOC("STAT_MONTH"), MONTH, &Date::month, stat);
		createChild<CommandStatDateField>(LOC("STAT_DAY"), DAY, &Date::day, stat);
		createChild<CommandStatDateField>(LOC("STAT_HOUR"), HOUR, &Date::hour, stat);
		createChild<CommandStatDateField>(LOC("STAT_MINUTE"), MINUTE, &Date::minute, stat);
		createChild<CommandStatDateField>(LOC("STAT_SECOND"), SECOND, &Date::second, stat);
		createChild<CommandStatDateField>(LOC("STAT_MILLISECOND"), MILLISECOND, &Date::millisecond, stat);

		useCommandNamesFromMenuName();
	}
}