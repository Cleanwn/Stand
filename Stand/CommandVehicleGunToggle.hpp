#pragma once

#include "CommandToggle.hpp"

#include "CommandListSelectVehicleModel.hpp"
#include "gta_vehicle.hpp"
#include "using_model.hpp"

namespace Stand
{
	class CommandVehicleGunToggle : public CommandToggle
	{
	private:
		std::vector<std::pair<AbstractEntity, time_t>> vehs;

		[[nodiscard]] Vector3 getCameraOffset() const
		{
			const auto rot = CAM::GET_FINAL_RENDERED_CAM_ROT(2);
			const auto pos = CAM::GET_FINAL_RENDERED_CAM_COORD();
			const auto dir = rot.toDir() * (float)distance->value;
			return pos + dir;
		}

		void doThing(Hash hash)
		{
			auto rot = CAM::GET_GAMEPLAY_CAM_ROT(0);
			auto pos = getCameraOffset();
			auto veh = Util::createVehicle(hash, pos, rot.z);

			if (veh.isValid())
			{
				ENTITY::SET_ENTITY_COORDS_NO_OFFSET(veh, pos.x, pos.y, pos.z, false, false, false);
				ENTITY::SET_ENTITY_ROTATION(veh, rot.x, rot.y, rot.z, 0, true);
				ENTITY::SET_ENTITY_NO_COLLISION_ENTITY(veh, g_player_ent, true);
				VEHICLE::SET_VEHICLE_FORWARD_SPEED(veh, (float)speed->value);

				SOUP_IF_LIKELY (auto cveh = veh.getCVehicle())
				{
					// This reduces the likelihood that this feature overwhelms its own visiblity with explosion spam.
					// Vehicles will still explode, but only moderately.
					cveh->m_nVehicleFlags.bTakeLessDamage = true;
				}

				veh.setCanMigrate(false);
				vehs.emplace_back(std::make_pair(veh, get_current_time_millis()));
			}
		}

	public:
		CommandSlider* speed = nullptr;
		CommandSlider* distance = nullptr;
		CommandListSelectVehicleModel* model = nullptr;

		explicit CommandVehicleGunToggle(CommandList* const parent)
			: CommandToggle(parent, LOC("VEHGUN"), CMDNAMES_OBF("vehiclegun"), LOC("VEHGUN_H"))
		{
		}

		void onEnable(Click& click) final
		{
			registerScriptTickEventHandler([this]
			{
				HANDLER_CHECK(this->m_on);

				const bool rand = this->model->value == CommandListSelectVehicleModel::RANDOM;
				const auto hash = this->model->getVehicle();

				if (!rand && !STREAMING::HAS_MODEL_LOADED(hash))
				{
					STREAMING::REQUEST_MODEL(hash);
				}

				if (PED::IS_PED_SHOOTING(g_player_ped))
				{
					if (STREAMING::HAS_MODEL_LOADED(hash))
					{
						doThing(hash);
					}
					else
					{
						FiberPool::queueJob([=]
						{
							using_model(hash, [this, hash]
							{
								doThing(hash);
							});
						});
					}
				}

				if (g_gui.isUnloadPending())
				{
					for (auto& [veh, _] : vehs)
					{
						veh.removeFromPlaneOfExistence();
					}

					vehs.clear();
				}
				else
				{
					for (auto i = vehs.begin(); i != vehs.end(); )
					{
						auto& veh = (*i).first;
						auto created = (*i).second;

						if (!veh.isValid())
						{
							i = vehs.erase(i);
						}
						else if (vehs.size() > 40 || GET_MILLIS_SINCE(created) > 120'000)
						{
							veh.removeFromPlaneOfExistence();
							i = vehs.erase(i);
						}
						else
						{
							if (GET_MILLIS_SINCE(created) < 2'000)
							{
								ENTITY::SET_ENTITY_NO_COLLISION_ENTITY(veh, g_player_ent, TRUE);
							}

							++i;
						}
					}
				}

				HANDLER_END;
			});
		}
	};
}