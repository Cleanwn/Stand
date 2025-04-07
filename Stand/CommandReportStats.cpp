#include "CommandReportStats.hpp"

#include <memory>

#include <fmt/xchar.h>

#include "atStringHash.hpp"
#include "CommandReadonlyValueLambda.hpp"
#include "Gui.hpp"
#include "IncrementableStat.hpp"
#include "lang.hpp"
#include "natives.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	struct Stat
	{
		const hash_t name;
		const Hash hash;
		const bool is_bool = false;
	};

	inline constexpr Stat stats[] = {
		{ATSTRINGHASH("ISE_G2"), STAT_GRIEFING},
		{ATSTRINGHASH("ISE_X2"), STAT_EXPLOITS},
		{ATSTRINGHASH("ISE_B2"), STAT_GAME_EXPLOITS},
		{ATSTRINGHASH("ISE_TC_A"), STAT_TC_ANNOYINGME},
		{ATSTRINGHASH("ISE_TC_H"), STAT_TC_HATE},
		{ATSTRINGHASH("ISE_VC_A"), STAT_VC_ANNOYINGME},
		{ATSTRINGHASH("ISE_VC_H"), STAT_VC_HATE},
		{ATSTRINGHASH("STAT_OFFENSIVE_LANGUAGE"), STAT_OFFENSIVE_LANGUAGE},
		{ATSTRINGHASH("STAT_OFFENSIVE_TAGPLATE"), STAT_OFFENSIVE_TAGPLATE},
		{ATSTRINGHASH("STAT_OFFENSIVE_UGC"), STAT_OFFENSIVE_UGC},
		{ATSTRINGHASH("STAT_BAD_CREW_NAME"), STAT_BAD_CREW_NAME},
		{ATSTRINGHASH("STAT_BAD_CREW_MOTTO"), STAT_BAD_CREW_MOTTO},
		{ATSTRINGHASH("STAT_BAD_CREW_STATUS"), STAT_BAD_CREW_STATUS},
		{ATSTRINGHASH("STAT_BAD_CREW_EMBLEM"), STAT_BAD_CREW_EMBLEM},
		{ATSTRINGHASH("ISE_F"), STAT_HELPFUL},
		{ATSTRINGHASH("ISE_H"), STAT_FRIENDLY},
		{ATSTRINGHASH("STAT_IS_CHEATER"), ATSTRINGHASH("MPPLY_IS_CHEATER"), true},
		{ATSTRINGHASH("STAT_WAS_I_BAD_SPORT"), ATSTRINGHASH("MPPLY_WAS_I_BAD_SPORT"), true},
		{ATSTRINGHASH("STAT_IS_HIGH_EARNER"), ATSTRINGHASH("MPPLY_IS_HIGH_EARNER"), true},
	};

	CommandReportStats::CommandReportStats(CommandList* const parent)
		: CommandListReadonly(parent, LOC("VWRPTSTAT"), { CMDNAME("reportstats") })
	{
	}

	void CommandReportStats::onClick(Click& click)
	{
		if (this->children.empty())
		{
			if (!g_gui.user_was_in_online)
			{
				click.setResponse(LOC("CMDONLONC"));
				return;
			}
			for (const Stat& stat : stats)
			{
				this->createChild<CommandReadonlyValueLambda>(LOC_RT(stat.name), [stat]() -> std::wstring
				{
					int value;
					if (!STATS::STAT_GET_INT(stat.hash, &value, -1))
					{
						return L"Failed to read";
					}
					return stat.is_bool ? StringUtils::utf8_to_utf16(Lang::get(value ? ATSTRINGHASH("YES") : ATSTRINGHASH("NO"))) : fmt::to_wstring(value);
				});
			}
		}
		CommandList::onClick(click);
	}
}
