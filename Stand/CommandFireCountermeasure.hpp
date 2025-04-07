#pragma once

#include "CommandAction.hpp"

#include "gta_vehicle.hpp"
#include "Particles.hpp"

namespace Stand
{
	class CommandFireCountermeasure : public CommandAction
	{
	public:
		enum CountermeasureType : uint8_t
		{
			NONE,
			CHAFF,
			FLARE,
			BOTH
		};

		enum FlarePattern : uint8_t
		{
			HORIZONTAL,
			HORIZONTAL_UPWARDS,
			HORIZONTAL_DOWNARDS,
			VERTICAL,
			VERTICAL_DOWNWARDS,
			RANDOM
		};

	private:
		bool chaff_busy = false;
		CommandSlider* const count;
		CommandToggle* const onlyair;
		CommandListSelect* const pattern;
		CountermeasureType cm_type;

		[[nodiscard]] Vector3 getPattern()
		{
			switch ((FlarePattern)pattern->value)
			{
			case HORIZONTAL:
				return Vector3(5.0f, -5.0f, 0.0f);

			case HORIZONTAL_UPWARDS:
				return Vector3(5.0f, -5.0f, 5.0f);

			case HORIZONTAL_DOWNARDS:
				return Vector3(5.0f, -5.0f, -5.0f);

			case VERTICAL:
				return Vector3(1.0f, -1.0f, 5.0f);

			case VERTICAL_DOWNWARDS:
				return Vector3(1.0f, -1.0f, -5.0f);
			}

			return Vector3(
				(float)soup::rand(1, 5),
				(float)soup::rand(1, 5) * -1.0f,
				(float)soup::rand(0, 5) * (soup::rand.coinflip() ? -1.0f : 1.0f)
			);
		}

	public:
		explicit CommandFireCountermeasure(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, CommandSlider* const count, CommandToggle* const onlyair, CommandListSelect* const pattern, CountermeasureType cm_type)
			: CommandAction(parent, std::move(menu_name), std::move(command_names), NOLABEL), count(count), onlyair(onlyair), pattern(pattern), cm_type(cm_type)
		{
		}

		void onClick(Click& click) final
		{
			click.setNoResponse();

			if (!shouldDeploy())
			{
				return;
			}

			if (g_player_veh.isValid()
				&& g_player_veh.isOwnerOfVehicleAndDriver()
				)
			{
				deployRelevant(this->cm_type);
			}
		}

		void deployFlares()
		{
			FiberPool::queueJob([this]
			{
				STREAMING::REQUEST_MODEL(ATSTRINGHASH("w_pi_flaregun_shell"));
				while (!STREAMING::HAS_MODEL_LOADED(ATSTRINGHASH("w_pi_flaregun_shell")))
				{
					Script::current()->yield();
				}

				const auto count = this->count->value;

				AUDIO::PLAY_SOUND_FROM_ENTITY(-1, "flares_released", g_player_veh, "DLC_SM_Countermeasures_Sounds", TRUE, 0);

				for (uint8_t i = 0; i != count; ++i)
				{
					const auto pat = getPattern();
					const auto pos = g_player_veh.getPos();
					const auto off1 = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(g_player_veh, -pat.x, pat.y, pat.z);
					const auto off2 = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(g_player_veh, pat.x, pat.y, pat.z);

					MISC::SHOOT_SINGLE_BULLET_BETWEEN_COORDS(pos.x, pos.y, pos.z, off1.x, off1.y, off1.z, 1, TRUE, ATSTRINGHASH("WEAPON_FLAREGUN"), g_player_ped, TRUE, TRUE, 1.0f);
					Script::current()->yield(25);
					MISC::SHOOT_SINGLE_BULLET_BETWEEN_COORDS(pos.x, pos.y, pos.z, off2.x, off2.y, off2.z, 1, TRUE, ATSTRINGHASH("WEAPON_FLAREGUN"), g_player_ped, TRUE, TRUE, 1.0f);
					Script::current()->yield(200);
				}
			});
		}

		void deployChaff()
		{
			FiberPool::queueJob([this]
			{
				if (chaff_busy)
				{
					return;
				}
				if (auto cveh = g_player_veh.getCVehicle())
				{
					Particles::play(reinterpret_cast<long long>(&g_particle_chaff), g_player_veh.getPos(), 5.0f);
					if (cveh->m_bAllowHomingMissleLockOnSynced) // The "chaff expiration" logic could intefere with "Can't Be Locked On"
					{
						chaff_busy = true;
						cveh->m_bAllowHomingMissleLockOnSynced = false;
						Script::current()->yield(3000);
						cveh->m_bAllowHomingMissleLockOnSynced = true;
						chaff_busy = false;
					}
					else
					{
						chaff_busy = true;
						Script::current()->yield(3000);
						chaff_busy = false;
					}
				}
			});
		}

		void deployRelevant(CountermeasureType type)
		{
			if (type != FLARE)
			{
				deployChaff();
			}

			if (type != CHAFF)
			{
				deployFlares();
			}
		}

		[[nodiscard]] bool shouldDeploy() const
		{
			if (onlyair->m_on)
			{
				const auto model = g_player_veh.getModel();
				return model.isAirborne() || model == ATSTRINGHASH("oppressor2");
			}

			return true;
		}
 	};
}