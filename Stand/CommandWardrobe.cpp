#include "CommandWardrobe.hpp"

#include <filesystem>

#include "AbstractEntity.hpp"
#include "alphanum.hpp"
#include "CommandLambdaAction.hpp"
#include "CommandLockOutfit.hpp"
#include "CommandSearchLiterals.hpp"
#include "FiberPool.hpp"
#include "natives.hpp"
#include "Script.hpp"
#include "StringUtils.hpp"

#include "CommandOutfitPreset.hpp"
#include "CommandSaveOutfit.hpp"
#include "CommandOutfit.hpp"

namespace Stand
{
	CommandWardrobe::CommandWardrobe(CommandList* parent)
		: CommandListFolder(parent, L"Outfits", { L".txt" }, LOC("WRDR"), { CMDNAME("wardrobe") })
	{
		CommandPhysical::collapse_command_names.emplace(L"outfit");
	}

	void CommandWardrobe::populateHead()
	{
		createChildWithBuilder<CommandSaveOutfit>();

		// Prefabricated Outfits
		{
			auto presets = this->createChild<CommandList>(LOC("OP"));

			presets->createChild<CommandLambdaAction>(LOC("OP_D"), { CMDNAME("defaultoutfit") }, LOC("OP_D_H"), [](Click& click)
			{
				click.ensureScriptThread([]
				{
					CommandLockOutfit::instance->updateOutfit([]
					{
						PED::SET_PED_DEFAULT_COMPONENT_VARIATION(g_player_ped);
						PED::CLEAR_ALL_PED_PROPS(g_player_ped, 1);
					});
				});
			});
			presets->createChild<CommandLambdaAction>(LOC("OP_RNG"), { CMDNAME("randomoutfit") }, NOLABEL, [](Click& click)
			{
				click.ensureScriptThread([]
				{
					CommandLockOutfit::instance->updateOutfit([]
					{
						g_player_ped.randomiseOutfit();
					});
				});
			});
			presets->createChild<CommandOutfitPreset>(LOC("OP_0"), { CMDNAME("toplessfemale") }, std::unordered_map<std::string, std::string>{
				{ "Model", "Online Female"} ,
				{ "Gloves / Torso", "15" },
				{ "Gloves / Torso Variation", "0" },
				{ "Top", "260" },
				{ "Top Variation", "0" },
				{ "Top 2", "2" },
				{ "Top 2 Variation", "0" },
			});
			presets->createChild<CommandOutfitPreset>(LOC("OP_COP_M"), { CMDNAME("copmale") }, std::unordered_map<std::string, std::string>{
				{ "Model", "Online Male"},
				{ "Mask", "0" },
				{ "Mask Variation", "0" },
				{ "Top", "55" },
				{ "Top Variation", "0" },
				{ "Gloves / Torso", "0" },
				{ "Gloves / Torso Variation", "0" },
				{ "Top 2", "58" },
				{ "Top 2 Variation", "0" },
				{ "Top 3", "0" },
				{ "Top 3 Variation", "0" },
				{ "Parachute / Bag", "0" },
				{ "Parachute / Bag Variation", "0" },
				{ "Pants", "35" },
				{ "Pants Variation", "0" },
				{ "Shoes", "25" },
				{ "Shoes Variation", "0" },
				{ "Accessories", "0" },
				{ "Accessories Variation", "0" },
				{ "Decals", "0" },
				{ "Decals Variation", "0" },
				{ "Hat", "46" },
				{ "Hat Variation", "0" },
				{ "Glasses", "13" },
				{ "Glasses Variation", "0" },
				{ "Earwear", "-1" },
				{ "Earwear Variation", "-1" },
				{ "Watch", "-1" },
				{ "Watch Variation", "-1" },
				{ "Bracelet", "-1" },
				{ "Bracelet Variation", "-1" },
			});
			presets->createChild<CommandOutfitPreset>(LOC("OP_COP_F"), { CMDNAME("copfemale") }, std::unordered_map<std::string, std::string>{
				{ "Model", "Online Female"},
				{ "Mask", "0" },
				{ "Mask Variation", "0" },
				{ "Top", "48" },
				{ "Top Variation", "0" },
				{ "Gloves / Torso", "14" },
				{ "Gloves / Torso Variation", "0" },
				{ "Top 2", "35" },
				{ "Top 2 Variation", "0" },
				{ "Top 3", "0" },
				{ "Top 3 Variation", "0" },
				{ "Parachute / Bag", "0" },
				{ "Parachute / Bag Variation", "0" },
				{ "Pants", "34" },
				{ "Pants Variation", "0" },
				{ "Shoes", "25" },
				{ "Shoes Variation", "0" },
				{ "Accessories", "0" },
				{ "Accessories Variation", "0" },
				{ "Decals", "0" },
				{ "Decals Variation", "0" },
				{ "Hat", "45" },
				{ "Hat Variation", "0" },
				{ "Glasses", "11" },
				{ "Glasses Variation", "0" },
				{ "Earwear", "-1" },
				{ "Earwear Variation", "-1" },
				{ "Watch", "-1" },
				{ "Watch Variation", "-1" },
				{ "Bracelet", "-1" },
				{ "Bracelet Variation", "-1" },
			});
		}

		createOpenFolderAction();
		createChild<CommandSearchLiterals<true>>(CMDNAMES("searchoutfit"));
	}

	CommandListFolderItemData* CommandWardrobe::getItemData(Command* cmd) const noexcept
	{
		return getItemDataImpl<CommandOutfit>(cmd);
	}

	void CommandWardrobe::onItemIsKept(Command* cmd) const
	{
		if (cmd->type != COMMAND_LIST)
		{
			((CommandOutfit*)cmd)->outfit.clear();
		}
	}

	void CommandWardrobe::createItem(CommandList* list, std::wstring&& filename_no_ext, CommandListFolderItemData&& data) const
	{
		list->createChild<CommandOutfit>(std::move(filename_no_ext), std::move(data));
	}

	void CommandWardrobe::sortImpl()
	{
		return sortBodyRecursively([](const std::unique_ptr<Command>& a, const std::unique_ptr<Command>& b)
		{
			if (a->type == COMMAND_LIST)
			{
				if (b->type == COMMAND_LIST)
				{
					return sortAlgoName(a, b);
				}
				return true;
			}
			if (b->type == COMMAND_LIST)
			{
				if (a->type == COMMAND_LIST)
				{
					return sortAlgoName(a, b);
				}
				return false;
			}
			auto a_name = ((CommandOutfit*)a.get())->outfit_name;
			auto b_name = ((CommandOutfit*)b.get())->outfit_name;
			StringUtils::simplify(a_name);
			StringUtils::simplify(b_name);
			return alphanum_less(a_name, b_name);
		});
	}
}
