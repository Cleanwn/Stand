#pragma once

#include "CommandWithOnTickFocused.hpp"

#include "AbstractEntity.hpp"
#include "AbstractModel.hpp"
#include "Box.hpp"
#include "CommandEntityPreviews.hpp"
#include "ePedConfigFlags.hpp"
#include "FiberPool.hpp"
#include "natives.hpp"
#include "Renderer.hpp"

//#include "drawDebugText.hpp"

namespace Stand
{
	template <typename T>
	class CommandWithEntityPreview : public CommandWithOnTickFocused<T>
	{
	protected:
		using CommandWithOnTickFocused<T>::CommandWithOnTickFocused;

	public:
		~CommandWithEntityPreview() override
		{
			getRidOfPreview();
		}

		void onTickFocused() override
		{
			if (!isPreviewEnabled())
			{
				getRidOfPreview();
				return;
			}

			if (!g_preview_ent.isValid())
			{
				auto model_hash = getModelHash();
				if (model_hash == 0 || AbstractModel(model_hash).isWaterAnimal())
				{
					return;
				}
				if (!STREAMING::HAS_MODEL_LOADED(model_hash))
				{
					STREAMING::REQUEST_MODEL(model_hash);
					return;
				}
				g_preview_ent = createPreviewEntity();
				g_preview_ent.ensureHandleAndPointer();
				v3 rot{};
				rot.z = CAM::GET_FINAL_RENDERED_CAM_ROT(2).z;
				if (g_preview_ent.isVehicle())
				{
					rot.z += 90.0f;
				}
				else if (g_preview_ent.isPed())
				{
					rot.z += 180.0f;
					g_preview_ent.godmodeEnable();
				}
				g_preview_ent.setRot(rot);
			}
			else
			{
				if (!g_preview_ent.exists())
				{
					g_preview_ent.reset();
					return;
				}
				if (!g_preview_ent.isObject())
				{
					v3 rot{};
					rot.z = g_preview_ent.getRot().z + CommandEntityPreviews::getRotationSpeed();
					g_preview_ent.setRot(rot);
				}
			}

			ENTITY::SET_ENTITY_HAS_GRAVITY(g_preview_ent, false);
			ENTITY::SET_ENTITY_COMPLETELY_DISABLE_COLLISION(g_preview_ent, false, false);
			ENTITY::SET_CAN_CLIMB_ON_ENTITY(g_preview_ent, false);

			if (!g_preview_ent.isObject())
			{
				int alpha = 255;
				if (!CommandEntityPreviews::isOpaque())
				{
					float speed_perc = 0.0f;
					if (g_player_veh.isValid() && g_player_veh.getDriver() == g_player_ped)
					{
						speed_perc = (ENTITY::GET_ENTITY_SPEED(g_player_ped) / 80.0f);
						if (speed_perc > 1.0f)
						{
							speed_perc = 1.0f;
						}
					}
					alpha = 200 - static_cast<int>(speed_perc * 150.0f);
				}
				ENTITY::SET_ENTITY_ALPHA(g_preview_ent, alpha, FALSE);
			}

			if (g_preview_ent.isVehicle())
			{
				VEHICLE::SET_VEHICLE_PROVIDES_COVER(g_preview_ent, false);
				g_preview_ent.setVehicleCanBeDamaged(false);
				g_preview_ent.repair();
			}
			else if (g_preview_ent.isPed())
			{
				PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(g_preview_ent, TRUE);
				PED::SET_PED_CONFIG_FLAG(g_preview_ent, CPED_CONFIG_FLAG_DisableExplosionReactions, TRUE);
				g_preview_ent.revive();
			}
			g_preview_ent.setOrigin(getEntityOrigin(g_preview_ent));
			g_preview_ent.getBox().drawLines(int(g_renderer.arColour.x * 255.0f), int(g_renderer.arColour.y * 255.0f), int(g_renderer.arColour.z * 255.0f), int(g_renderer.arColour.w * 200.0f));
		}

		void onBlur(ThreadContext thread_context, Direction momentum) override
		{
			CommandWithOnTickFocused<T>::onBlur(thread_context, momentum);
			getRidOfPreview();
		}

	protected:
		[[nodiscard]] virtual bool isPreviewEnabled() const noexcept
		{
			return !CommandEntityPreviews::isDisabled();
		}

		[[nodiscard]] virtual Hash getModelHash() = 0;
		[[nodiscard]] virtual AbstractEntity createPreviewEntity() = 0;

		[[nodiscard]] virtual v3 getEntityOrigin(AbstractEntity& ent) const
		{
			auto forward_vector = CAM::GET_FINAL_RENDERED_CAM_ROT(2).toDir() * (ent.getDimensions().max() + CommandEntityPreviews::getAdditionalOffset(ent.getModel()));
			/*if (ent.is_object())
			{
				auto dist = forward_vector.distanceSquared(rage::scrVector{});
				//drawDebugText(dist);
				if (dist < 20.0f)
				{
					forward_vector *= 5.0f;
				}
				else if (dist < 100.0f)
				{
					forward_vector *= 2.0f;
				}
			}*/
			return CAM::GET_FINAL_RENDERED_CAM_COORD() + forward_vector;
		}

		void getRidOfPreview()
		{
			if (g_preview_ent.isValid())
			{
				auto ent = g_preview_ent;
				g_preview_ent.reset();
				FiberPool::queueJob([ent]() mutable
				{
					auto model = ent.getModel();
					ent.removeFromPlaneOfExistence();
					STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(model);
				});
			}
		}
	};
}
