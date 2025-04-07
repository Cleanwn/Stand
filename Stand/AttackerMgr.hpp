#pragma once

#include "AbstractEntity.hpp"
#include "AbstractPlayer.hpp"
#include "ChainedVehicleMgr.hpp"
#include "get_current_time_millis.hpp"
#include "gta_net_game_event.hpp"

namespace Stand
{
	enum AttackerType : uint8_t
	{
		NORMAL,
		PHANTOM,
	};

	class AttackerMgr
	{
	private:
		struct AttackerGroup
		{
			AbstractEntity attacker;
			AbstractPlayer victim;
			AbstractEntity vehicle;

			AttackerType type = NORMAL;

			v3 last_position = {};
			time_t last_position_check = 0;

			int sound_id = -1;
			int ptfx_handle = -1;

			void pop()
			{
				if (sound_id != -1)
				{
					AUDIO::RELEASE_SOUND_ID(sound_id);
				}

				if (ptfx_handle != -1)
				{
					GRAPHICS::STOP_PARTICLE_FX_LOOPED(ptfx_handle, FALSE);
				}

				vehicle.removeFromPlaneOfExistence();
				attacker.removeFromPlaneOfExistence();
			}

			void processPlanePositionUpdate()
			{
				if (GET_MILLIS_SINCE(last_position_check) > 3000)
				{
					v3 current_position = vehicle.getPos();

					if (!last_position.isNull()
						&& last_position.distance(current_position) < 20.0f // Probably stuck.
						)
					{
						if (attacker.isInvulnerable()) // Let them die if they're not supposed to live forever.
						{
							current_position = ChainedVehicleMgr::randomiseAbove(current_position);
							vehicle.setPos(current_position);
							vehicle.setRot(v3(0.0f, 0.0f, 0.0f));
							vehicle.repair();
							vehicle.startEngine();
							VEHICLE::SET_VEHICLE_FORWARD_SPEED(vehicle, 60.0f);
						}
					}

					last_position = current_position;
					last_position_check = get_current_time_millis();
				}
			}
 		};

		static inline std::vector<AttackerGroup> attackers{};
		static inline std::unordered_set<compactplayer_t> locked_players{};

	public:
		static void push(AbstractEntity& attacker, AbstractPlayer victim, Hash weapon, const bool immortality, const Hash vehicle_hash, const bool fill_vehicles, AttackerType type = AttackerType::NORMAL);
		static void clear(compactplayer_t p);

		[[nodiscard]] static bool canCreateAttackers(compactplayer_t p);
		[[nodiscard]] static int getAccuracyForPlayer(const AbstractPlayer& player);

		static void customiseVehicle(AbstractEntity& veh, AttackerType type);
		static void customiseAttacker(AbstractEntity& ped, AttackerType type);

		static void applyTasks(AbstractEntity& driver, AbstractEntity& veh, AbstractEntity& victim, AttackerType type);

		static void onTick();
		static void onTickVehicle(AttackerGroup& group);
		static void onTickAttacker(AttackerGroup& group);

		static void initAudioBanks(AttackerType type);
		[[nodiscard]] static std::vector<Hash> getAudioBankDependencies(AttackerType type) noexcept;

		[[nodiscard]] static bool isEntityAttacker(AbstractEntity& entity);

		static void lockAttackerCreation(compactplayer_t p);
		static void unlockAttackerCreation(compactplayer_t p);
	};
}
