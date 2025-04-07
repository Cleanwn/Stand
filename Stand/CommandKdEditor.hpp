#pragma once

#include "CommandStatSlider.hpp"

#include "as_fraction.hpp"
#include "Stat.hpp"

namespace Stand
{
	class CommandKdEditor : public CommandStatSlider<CommandSliderFloat>
	{
	private:
		static inline Stat KDR = Stat("MPPLY_KILL_DEATH_RATIO", false, Stats::FLOAT);

	public:
		explicit CommandKdEditor(CommandList* const parent)
			: CommandStatSlider(parent, KDR, -INT32_MAX, INT32_MAX)
		{
			setMenuName(LOC("KDEDIT"));
			setHelpText(LOC("KDEDIT_H"));
			setCommandNames(CMDNAMES_OBF("setkd"));
		}

	protected:
		void setStat(Hash stat) final
		{
			const auto fraction = as_fraction((double)getFloatValue());

			STATS::STAT_SET_INT(ATSTRINGHASH("MPPLY_KILLS_PLAYERS"), fraction.first, TRUE);
			STATS::STAT_SET_INT(ATSTRINGHASH("MPPLY_DEATHS_PLAYER"), fraction.second, TRUE);
			STATS::STAT_SET_FLOAT(ATSTRINGHASH("MPPLY_KILL_DEATH_RATIO"), getFloatValue(), TRUE);
		}

		void getStat(Hash stat, int* value) final
		{
			float v = 0.0f;
			STATS::STAT_GET_FLOAT(ATSTRINGHASH("MPPLY_KILL_DEATH_RATIO"), &v, -1);
			*value = (int)(v * getPrecisionScalar());
		}
	};
}