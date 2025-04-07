#include "CommandListRadioStations.hpp"

#include "CommandListSelect.hpp"
#include "is_session.hpp"
#include "natives.hpp"
#include "pointers.hpp"
#include "radio_stations.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandRadioStation : public CommandListSelect
	{
	private:
		const char* const station_name;

	public:
		explicit CommandRadioStation(CommandList* const parent, const char* station_name)
			: CommandListSelect(parent, LIT(Util::GET_LABEL_TEXT(station_name, false)), {}, NOLABEL, {
				{0, LOC("OVRDOFF")},
				{1, LOC("SHW")},
				{2, LOC("HDE")},
			}, 0, CMDFLAGS_LIST_SELECT, (strcmp(station_name, "RADIO_11_TALK_02") == 0 ? 0 : CMDFLAG_FEATURELIST_FINISHLIST)), station_name(station_name)
		{
		}

		static void lock_radio_station(const char* station_name, bool toggle)
		{
			if (pointers::lock_radio_lsur_hash)
			{
				*pointers::lock_radio_lsur_hash = 0;
			}
			AUDIO::LOCK_RADIO_STATION(station_name, toggle);
			if (pointers::lock_radio_lsur_hash)
			{
				*pointers::lock_radio_lsur_hash = ATSTRINGHASH("RADIO_22_DLC_BATTLE_MIX1_RADIO");
			}
		}

		void onChange(Click& click, long long prev_value) final
		{
			if (value == 0)
			{
				if (strcmp(station_name, "RADIO_11_TALK_02") == 0)
				{
					if (pointers::radio_talk02_hash)
					{
						*pointers::radio_talk02_hash = ATSTRINGHASH("RADIO_11_TALK_02");
					}
				}
				return;
			}
			if (!click.isRegularEdition())
			{
				return onChangeImplUnavailable(click, prev_value);
			}
			if (strcmp(station_name, "RADIO_11_TALK_02") == 0)
			{
				if (pointers::radio_talk02_hash)
				{
					*pointers::radio_talk02_hash = 0;
				}
			}
			if (value == 1)
			{
				registerScriptTickEventHandler(click, [this]
				{
					lock_radio_station(station_name, false);
					if (is_session_started())
					{
						AUDIO::SET_RADIO_STATION_AS_FAVOURITE(station_name, true);
					}
					return value == 1;
				});
			}
			else if (value == 2)
			{
				registerScriptTickEventHandler(click, [this]
				{
					lock_radio_station(station_name, true);
					if (is_session_started())
					{
						AUDIO::SET_RADIO_STATION_AS_FAVOURITE(station_name, false);
					}
					return value == 2;
				});
			}
		}
	};

	CommandListRadioStations::CommandListRadioStations(CommandList* const parent)
		: CommandList(parent, LOC("RDSTS"))
	{
		for (const auto& radio_station : radio_stations)
		{
			this->createChild<CommandRadioStation>(radio_station);
		}
	}
}
