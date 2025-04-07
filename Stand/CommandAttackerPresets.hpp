#pragma once

#include "CommandOnPlayer.hpp"
#include "CommandListAction.hpp"

#include "AttackerMgr.hpp"

namespace Stand
{
	class CommandAttackerPresets : public CommandOnPlayer<CommandListAction>
	{
	private:
		struct AttackerPreset
		{
			Hash ped;
			std::vector<Hash> vehicles;

			Label menu_label;
			Hash weapon = 0;

			AttackerType type = AttackerType::NORMAL;
		};

		static inline AttackerPreset presets[] = {
			AttackerPreset{ ATSTRINGHASH("csb_mweather"), { ATSTRINGHASH("strikeforce") }, LOC("ATTRKS_PRE_1") },
			AttackerPreset{ ATSTRINGHASH("a_m_m_genbiker_01"), { ATSTRINGHASH("daemon") }, LOC("ATTRKS_PRE_2") },
			AttackerPreset{ ATSTRINGHASH("s_m_y_clown_01"), { ATSTRINGHASH("speedo2") }, LOC("ATTRKS_PRE_3") },
			AttackerPreset{ ATSTRINGHASH("csb_mweather"), { ATSTRINGHASH("valkyrie2") }, LOC("ATTRKS_PRE_4") },
			AttackerPreset{ ATSTRINGHASH("a_m_y_skater_01"), { ATSTRINGHASH("bmx") }, LOC("ATTRKS_PRE_5") },
			AttackerPreset{ ATSTRINGHASH("a_m_m_farmer_01"), { ATSTRINGHASH("tractor") }, LOC("ATTRKS_PRE_6") },
			AttackerPreset{ ATSTRINGHASH("a_m_y_methhead_01"), { ATSTRINGHASH("minitank") }, LOC("ATTRKS_PRE_7") },
			AttackerPreset{ ATSTRINGHASH("s_m_y_marine_03"), { ATSTRINGHASH("valkyrie2"), ATSTRINGHASH("lazer"), ATSTRINGHASH("halftrack"), ATSTRINGHASH("rhino"), ATSTRINGHASH("crusader") }, LOC("ATTRKS_PRE_8") },
			AttackerPreset{ ATSTRINGHASH("s_m_y_robber_01"), { ATSTRINGHASH("tornado5") }, LOC("ATTRKS_PRE_9"), ATSTRINGHASH("WEAPON_UNARMED"), AttackerType::PHANTOM },
		};

		[[nodiscard]] static std::vector<CommandListActionItemData> getOptions();

	public:
		explicit CommandAttackerPresets(CommandList* const parent);

		void onItemClick(Click& click, CommandListActionItem* item) final;
	};
}