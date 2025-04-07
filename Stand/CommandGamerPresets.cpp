#include "CommandGamerPresets.hpp"

#include "CommandGamerPreset.hpp"
#include "rockstar_admins.hpp"

namespace Stand
{
	CommandGamerPresets::CommandGamerPresets(CommandList* const parent, Label&& menu_name, bool include_rockstar_admins)
		: CommandList(parent, std::move(menu_name))
	{
		{
			static const std::pair<const char*, std::pair<int64_t, const char*>> youtubers_data[] = {
				{"DarkViperAU", {64839021, "RealDarkViperAU"}},
				{"MrBossFTW", {26204131, "MrBossFTW"}},
				{"Kwebbelkop", {91533847, "Kwebblekop"}},
				{"NoughtPointFourLive", {117730150, "NoughtPointFourLive"}},
				{"SpeirsTheAmazing", {19171376, "SpeirsTheAmazing"}},
				{"iXpertThief", {21986863, "iXpertThief"}},
				{"Motmus", {98598332, "Aurora1x"}},
				{"KSI", {31754771, "BlackYeah"}},
				{"Zerkaa", {26197589, "ZerkaaHD"}},
				{"TBJZL", {27278325, "TobiLerone"}},
				{"W2SPlays", {44902434, "wroetobeg"}},
				{"Vikkstar123", {46815882, "ItsMeVikkstar123"}},
				{"MM7Games", {31326553, "Miniminter7"}},
				{"GTA Gentleman", {109395899, "gtagentleman"}},
				{"Tylarious", {60199937, "Tykillington"}},
				{"Modest Pelican", {132509422, "Modest_Pelican"}},
				{"SomeOrdinaryGamers", {263527, "MomWorksForSega"}},
				{"Broughy1322", {234716, "Broughy1322"}},
				{"Gtamen", {62141, "Gtamen"}},
				{"Gypsy", {64161622, "GypsyYT"}},
				{"hella-flush", {27676838, "hella-flush"}},
				{"IsuckAtDriving", {61375255, "tigerrider24"}},
				{"Joshimuz", {9337347, "Joshimuz"}},
				{"MrBeast", {171799519, "JimmyMBG"}},
				{"GermanLetsPlay", {13970050, "T-GermanLetsPlay"}},
				{"maudado", {18179125, "maudado"}},
				{"LordZombey", {27850966, "LordZombey"}},
				{"Paluten", {31564369, "Paluten"}},
				{"ohBrox", {28069850, "ohBrox"}},
				{"Lusor_Marbossa", {64241622, "Lusor_Marbossa"}},
				{"Lusor_Deko", {63972950, "Lusor_Deko"}},
				{"lpmitkev", {31879712, "lpmitkev"}},
			};
			auto youtubers_list = this->createChild<CommandList>(LOC("YTRS"));
			for (const auto& youtuber : youtubers_data)
			{
				youtubers_list->createChild<CommandGamerPreset>(youtuber.first, &youtuber.second);
			}
		}
		
		if (include_rockstar_admins)
		{
			auto rockstar_admins_list = this->createChild<CommandList>(LOC("RSEMPS"));
			for (const auto& rockstar_admin : rockstar_admins)
			{
				rockstar_admins_list->createChild<CommandGamerPreset>(&rockstar_admin);
			}
		}
	}
}
