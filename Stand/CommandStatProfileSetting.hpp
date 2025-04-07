#pragma once

#include "CommandStatSlider.hpp"

namespace Stand
{
	class CommandStatProfileSetting : public CommandStatSlider<CommandSlider>
	{
	private:
		const int setting_id = 0;

	public:
		explicit CommandStatProfileSetting(CommandList* const parent, Stat& stat)
			: CommandStatSlider(parent, stat, -INT32_MAX, INT32_MAX), setting_id(stat.getProfileSettingId())
		{
		}

	protected:
		void setStat(Hash stat) final
		{
			STATS::STAT_SET_PROFILE_SETTING_VALUE(this->setting_id, this->value); // Does not seem to save as expected but the native already commented this so I'm not certain it's a problem.
		}

		void getStat(Hash stat, int* value) final
		{
			*value = MISC::GET_PROFILE_SETTING(this->setting_id);
		}
	};
}