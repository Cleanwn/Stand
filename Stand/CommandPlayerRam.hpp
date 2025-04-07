#pragma once

#include "CommandPlayerAction.hpp"

#include "CommandListSelectVehicleModel.hpp"
#include "using_model.hpp"

namespace Stand
{
	class CommandPlayerRam : public CommandPlayerAction
	{
	private:
		bool last_was_positive = false;

		[[nodiscard]] float getInvertedY()
		{
			float offsetY = last_was_positive ? -30.0f : 30.0f;
			last_was_positive = !last_was_positive;
			return offsetY;
		}

		[[nodiscard]] AbstractEntity createVehicle(Hash hash, const AbstractPlayer& player)
		{
			auto ped = player.getPed();
			auto pos = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(ped, 0.0f, getInvertedY(), 0.0f);
			auto vehicle = Util::createVehicle(hash, pos, ENTITY::GET_ENTITY_HEADING(ped) + 180.0f);
			vehicle.setRot(pos.lookAt(ped.getPos()));
			return vehicle;
		}

	public:
		bool invisible = false;
		CommandSlider* speed;
		CommandListSelectVehicleModel* model;

		explicit CommandPlayerRam(CommandList* const parent)
			: CommandPlayerAction(parent, LOC("RAM"), CMDNAMES_OBF("ram"))
		{
		}

		void onClick(Click& click) final
		{
			ensureScriptThread(click, [this]
			{
				const auto hash = model->getVehicle();
				const auto player = AbstractPlayer(getPlayer());
				if (player.hasPed())
				{
					using_model(hash, [this, hash, player]
					{
						auto vehicle = createVehicle(hash, player);

						VEHICLE::SET_VEHICLE_GRAVITY(vehicle, false);
						VEHICLE::SET_DISABLE_MAP_COLLISION(vehicle);
						VEHICLE::SET_VEHICLE_FORWARD_SPEED(vehicle, (float)speed->value);

						ENTITY::SET_ENTITY_VISIBLE(vehicle, !invisible, FALSE);

						FiberPool::queueJob([vehicle]() mutable
						{
							Script::current()->yield(2500);
							vehicle.removeFromPlaneOfExistence();
						});
					});
				}
			});
		}
	};
}