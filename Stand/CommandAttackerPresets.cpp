#include "CommandAttackerPresets.hpp"

#include "CommandListActionItem.hpp"
#include "CommandPlayerAttackers.hpp"

namespace Stand
{
	[[nodiscard]] std::vector<CommandListActionItemData> CommandAttackerPresets::getOptions()
	{
		std::vector<CommandListActionItemData> res;
		for (size_t i = 0; i != COUNT(presets); ++i)
		{
			res.emplace_back(CommandListActionItemData(i, presets[i].menu_label));
		}
		return res;
	}

	CommandAttackerPresets::CommandAttackerPresets(CommandList* const parent)
		: CommandOnPlayer(parent, LOC("PRSTS"), CMDNAMES_OBF("presets"), NOLABEL, getOptions())
	{
		PC_PTR->registerCommandNames(this);
	}

	void CommandAttackerPresets::onItemClick(Click& click, CommandListActionItem* item)
	{
		auto id = (uint8_t)item->value;
		ensureYieldableScriptThread([this, id]
		{
			const Hash& ped_hash = presets[id].ped;
			const std::vector<Hash>& vehicle_hashes = presets[id].vehicles;

			const auto attacker_list = this->parent->as<CommandPlayerAttackers>();

			STREAMING::REQUEST_MODEL(ped_hash);
			while (!STREAMING::HAS_MODEL_LOADED(ped_hash))
			{
				Script::current()->yield();
			}

			Hash weapon_hash = presets[id].weapon == 0 ? (Hash)attacker_list->weapon->value : presets[id].weapon;
			Hash vehicle_hash = vehicle_hashes.at(soup::rand.t<size_t>(0, vehicle_hashes.size() - 1));

			const auto player = PP_PTR->getPlayer();

			AttackerMgr::unlockAttackerCreation(player);
			for (uint8_t i = 0; i < attacker_list->count; ++i)
			{
				if (!AttackerMgr::canCreateAttackers(player))
				{
					return;
				}

				auto pos = player.getPos();
				auto ped = Util::createPed(PED_TYPE_ANIMAL, ped_hash, pos, 0);

				AttackerMgr::push(ped, player, weapon_hash, attacker_list->immortality, vehicle_hash, attacker_list->fill_vehicles, presets[id].type);

				if (vehicle_hashes.size() > 1)
				{
					int attempts = 0;
					Hash new_hash = vehicle_hash;
					while (new_hash == vehicle_hash)
					{
						if (++attempts == 10)
						{
							break;
						}

						new_hash = vehicle_hashes.at(soup::rand.t<size_t>(0, vehicle_hashes.size() - 1));
					}
					vehicle_hash = new_hash;
				}

				Script::current()->yield(5);
			}
		});
	}
}