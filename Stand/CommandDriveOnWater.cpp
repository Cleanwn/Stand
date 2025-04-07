#include "CommandDriveOnWater.hpp"

#include "AbstractModel.hpp"
#include "Util.hpp"

namespace Stand
{
	CommandDriveOnWater::CommandDriveOnWater(CommandList* const parent)
		: CommandToggle(parent, LOC("DRVONWTR"), CMDNAMES("driveonwater"))
	{
	}

	void CommandDriveOnWater::onEnable(Click& click)
	{
		registerScriptTickEventHandler([this]
		{
			HANDLER_CHECK(m_on);

			if (!STREAMING::HAS_MODEL_LOADED(object_hash))
			{
				STREAMING::REQUEST_MODEL(object_hash);
			}

			if (g_player_veh.isValid()
				&& !g_player_veh.getModel().isAquatic()
				&& g_player_veh.isOwnerOfVehicleAndDriver()
				)
			{
				auto pos = g_player_veh.getPos();

				if (const auto option = getWaterHeight(pos); option.has_value())
				{
					createOrUpdateObject(pos, option.value());

					SOUP_IF_LIKELY (object.isValid()) // Activating while partially submerged can glitch the camera through collision spam.
					{
						const auto obj_pos = object.getPos();
						if (pos.z < obj_pos.z)
						{
							pos.z = obj_pos.z + 0.1f;
							g_player_veh.setPos(pos);
						}
					}

					HANDLER_END;
				}
			}

			reset();

			HANDLER_END;
		});
	}

	void CommandDriveOnWater::onDisable(Click& click)
	{
		ensureScriptThread(click, [this]
		{
			reset();
			STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(object_hash);
		});
	}

	void CommandDriveOnWater::createOrUpdateObject(const v3& pos, float water_height)
	{
		// Offset stylistically chosen to produce water effects. After all we're supposed to drive on the water, not above it. 
		auto new_pos = v3(pos.x, pos.y, water_height - 0.30f);

		if (!object.isValid())
		{
			if (object = Util::createObject(object_hash, new_pos, false); object.isValid())
			{
				ENTITY::FREEZE_ENTITY_POSITION(object, TRUE);
				ENTITY::SET_ENTITY_VISIBLE(object, FALSE, FALSE);
				ENTITY::SET_ENTITY_NO_COLLISION_ENTITY(object, g_player_ped, FALSE); // The game will only check CPhysical::TestNoCollision if rage::fwDynamicEntityComponent::m_pNoCollisionEntity is valid.
			}
		}

		// If we're upside down, try to avoid unnecessarily repositioning the object. Each reposition invokes a new collision. Excessive collisions on our roof will accelerate us in random directions and prevent us from flipping.
		if (object.isValid()
			&& (!ENTITY::IS_ENTITY_UPSIDEDOWN(g_player_veh) || object.getPos().distanceTopdown(new_pos) > 5.0f)
			)
		{
			const auto old_pos = object.getPos();
			new_pos.z = soup::lerp(old_pos.z, new_pos.z, 0.1f);
			ENTITY::SET_ENTITY_COORDS_NO_OFFSET(object, new_pos.x, new_pos.y, new_pos.z, FALSE, FALSE, FALSE);

			auto rot = object.getRot();
			rot.z = g_player_veh.getRot().z;

			auto fwd = new_pos + (ENTITY::GET_ENTITY_FORWARD_VECTOR(g_player_veh) * 5.0f);
			if (auto height = getWaterHeight(fwd); height.has_value())
			{
				fwd.z = height.value();
				rot.x = soup::lerp(rot.x, new_pos.lookAt(fwd).x, 0.05f) - 0.075f; // Offset to keep the nose slightly lower. Looks more natural.
			}

			object.setRot(rot);
		}
	}

	std::optional<float> CommandDriveOnWater::getWaterHeight(const v3& pos)
	{
		float height = 0.0f;

		if (WATER::TEST_VERTICAL_PROBE_AGAINST_ALL_WATER(pos.x, pos.y, pos.z + 5.0f, 0, &height) != 0)
		{
			return height;
		}

		return std::nullopt;
	}

	void CommandDriveOnWater::reset()
	{
		if (object.isValid())
		{
			object.removeFromPlaneOfExistence();
			object.reset();
		}
	}
}