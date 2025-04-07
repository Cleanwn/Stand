#pragma once

#include "CommandList.hpp"

#include "AttackerMgr.hpp"
#include "CommandAttackerPresets.hpp"
#include "CommandDivider.hpp"
#include "CommandTogglePointer.hpp"
#include "CommandLambdaActionScript.hpp"
#include "CommandLambdaListSelect.hpp"
#include "CommandListSelectPedModels.hpp"
#include "CommandListSelectVehicleModel.hpp"
#include "CommandListSelectWeapons.hpp"
#include "CommandSlider.hpp"
#include "CommandSliderPointer.hpp"
#include "PedType.hpp"

namespace Stand
{
	class CommandPlayerAttackers : public CommandList
	{
	public:
		uint8_t count = 20;
		bool immortality = true;
		bool fill_vehicles = true;

		CommandListSelectWeapons* weapon = nullptr;
		CommandListSelectPedModels* model = nullptr;
		CommandListSelectVehicleModel* vehicle = nullptr;
		CommandSlider* accuracy = nullptr;

	private:
		void spawn()
		{
			const auto plr = PP_PTR->getPlayer();

			AttackerMgr::unlockAttackerCreation(plr);
			for (uint8_t i = 0; i < this->count; ++i)
			{
				if (!AttackerMgr::canCreateAttackers(plr))
				{
					return;
				}

				auto pos = plr.getPos();
				auto veh = this->vehicle->getVehicle();
				auto ped = this->model->createPed(plr.getPos(), PED_TYPE_ANIMAL, plr.getPed());

				const auto weapon = ped.getModel().isAnimal()
					? ATSTRINGHASH("weapon_animal")
					: (Hash)this->weapon->value
					;

				AttackerMgr::push(ped, plr, weapon, immortality, veh, fill_vehicles);
				Script::current()->yield(5);
			}
		}

	public:
		explicit CommandPlayerAttackers(CommandList* const parent, commandflags_t flags)
			: CommandList(parent, LOC("ATTKRS"), {}, NOLABEL, flags)
		{
			// Spawn
			{
				auto spawn = this->createChild<CommandList>(LOC("SPWN"));

				spawn->createChild<CommandLambdaActionScript>(LOC("SPWN"), {}, NOLABEL, [this](const Click& click)
				{
					this->spawn();
				});

				spawn->createChild<CommandSliderPointer<CommandSlider, uint8_t>>(LOC("COUNT"), {}, NOLABEL, &this->count, 1, 20, 1);

				this->model = spawn->createChild<CommandListSelectPedModels>({}, NOLABEL, ATSTRINGHASH("player"), ONLY_HOSTILE | NO_HIRES_PED);
				this->vehicle = spawn->createChild<CommandListSelectVehicleModel>(LOC("VEH"), {}, NOLABEL, CommandListSelectVehicleModel::NONE, true, true, std::vector<hash_t>{ ATSTRINGHASH("rail") }, CommandListSelectVehicleModel::Disallows::ZERO_SEATERS);
				this->weapon = spawn->createChild<CommandListSelectWeapons>({}, NOLABEL, ATSTRINGHASH("WEAPON_APPISTOL"));
				this->accuracy = spawn->createChild<CommandSlider>(LOC("ACCURACY"), {}, NOLABEL, 0, 100, 25, 10);
			}

			this->createChild<CommandAttackerPresets>();

			// Meta
			{
				auto meta = this->createChild<CommandDivider>(LOC("META"));

				this->createChild<CommandTogglePointer>(&this->immortality, LOC("GOD"));
				this->createChild<CommandTogglePointer>(&this->fill_vehicles, LOC("FILL_VEH"), {}, LOC("FILL_VEH_H"));
				this->createChild<CommandLambdaActionScript>(LOC("DEL_ATTKRS"), {}, NOLABEL, [this](const Click& click)
				{
					compactplayer_t p = PP_PTR->getPlayer();
					AttackerMgr::lockAttackerCreation(p);

					ensureYieldableScriptThread(click, [p]
					{
						Script::current()->yield(200 + 5);
						AttackerMgr::clear(p);
					});
				});
			}
		}
	};
}
