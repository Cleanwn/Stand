#pragma once

#include "CommandToggleRegularSp.hpp"

#include "get_ground_z.hpp"
#include "GhostDriver.hpp"

namespace Stand
{
	class CommandVehFollow : public CommandToggleRegularSp
	{
	private:
		GhostDriver controller{};
		time_t forward_sample_start = 0;
		v3 forward_sample_pos{};
		bool cant_advance_forward = false;
		time_t backward_sample_start = 0;
		v3 backward_sample_pos{};
		bool cant_advance_backwards = false;
		uint8_t stopping = 0;

	public:
		explicit CommandVehFollow(CommandList* const parent)
			: CommandToggleRegularSp(parent, LOC("VEHFOLLOW"), CMDNAMES("vehfollow"))
		{
		}

		void cleanup()
		{
			if (controller.needsCleanup())
			{
				controller.cleanup();
			}
		}

		void onEnable(Click& click) final
		{
			checkPermissions(click);
			registerScriptTickEventHandler(click, [this]
			{
				if (m_on)
				{
					AbstractEntity veh;
					if (GhostDriver::user_is_rcing
						|| !controller.initVehicle(veh)
						)
					{
						cleanup();
					}
					else
					{
						const auto veh_pos = veh.getPos();

						if ((veh_pos.z - get_ground_z(veh_pos.x, veh_pos.y, veh_pos.z)) < 1.0f)
						{
							AbstractEntity driver;
							controller.initDriver(driver);

							const auto veh_dims = veh.getDimensions();
							const auto veh_size = std::max(veh_dims.x, veh_dims.y);
							const auto player_pos = g_player_ped.getPos();
							const auto player_dims = g_player_ped.getDimensions();
							const auto player_size = std::max(player_dims.x, player_dims.y);
							const auto required_distance = std::max(veh_size, player_size);
							if (stopping == 0
								&& veh_pos.distance(player_pos) > required_distance
								)
							{
								if (VEHICLE::IS_THIS_MODEL_A_BIKE(veh.getModel()))
								{
									if (!VEHICLE::IS_VEHICLE_ON_ALL_WHEELS(veh))
									{
										VEHICLE::SET_VEHICLE_ON_GROUND_PROPERLY(veh, 5.0f);
									}
								}
								veh.startEngine();

								auto veh_rot = veh.getRot();
								auto pos_veh_front = veh_pos + v3(0.0f, 0.0f, veh_rot.z).toDir();
								auto pos_veh_back = veh_pos - v3(0.0f, 0.0f, veh_rot.z).toDir();
								auto pos_veh_right = veh_pos + v3(0.0f, 0.0f, veh_rot.z + 90.0f).toDir();
								auto pos_veh_left = veh_pos - v3(0.0f, 0.0f, veh_rot.z + 90.0f).toDir();

								auto dist_front = player_pos.distance(pos_veh_front);
								auto dist_back = player_pos.distance(pos_veh_back);
								auto dist_right = player_pos.distance(pos_veh_right);
								auto dist_left = player_pos.distance(pos_veh_left);

								bool is_right = (dist_right < dist_left);
								bool is_side = std::min(dist_right, dist_left) < std::min(dist_front, dist_back);
								bool is_front = (dist_front < dist_back) || (veh_pos.distance(player_pos) > 15.0f);
								auto d = Util::angles_dist(veh_pos.lookAt(player_pos).z, veh.getRot().z);

								if ((is_front && !cant_advance_forward) || cant_advance_backwards)
								{
									if (d > 30.0f)
									{
										if (is_right)
										{
											TASK::TASK_VEHICLE_TEMP_ACTION(driver, veh, 7, 60);
										}
										else
										{
											TASK::TASK_VEHICLE_TEMP_ACTION(driver, veh, 8, 60);
										}
									}
									else
									{
										TASK::TASK_VEHICLE_TEMP_ACTION(driver, veh, 9, 60);
									}

									if (GET_MILLIS_SINCE(forward_sample_start) > 1000)
									{
										if (forward_sample_start != 0)
										{
											cant_advance_forward = (forward_sample_pos.distance(veh_pos) < 0.5f);
										}
										forward_sample_start = get_current_time_millis();
										forward_sample_pos = veh_pos;
									}

									if (GET_MILLIS_SINCE(backward_sample_start) > 2000)
									{
										cant_advance_backwards = false;
									}
								}
								else
								{
									if (is_side || cant_advance_forward || (d > 150.0f && d < 170.0f))
									{
										if (!is_side && !cant_advance_forward && veh_pos.distance(player_pos) > 4.0f)
										{
											cant_advance_forward = true;
											forward_sample_start = get_current_time_millis() + 1000;
										}
										if (is_right ^ cant_advance_forward)
										{
											TASK::TASK_VEHICLE_TEMP_ACTION(driver, veh, 13, 60);
										}
										else
										{
											TASK::TASK_VEHICLE_TEMP_ACTION(driver, veh, 14, 60);
										}
									}
									else
									{
										TASK::TASK_VEHICLE_TEMP_ACTION(driver, veh, 28, 60);
									}

									if (GET_MILLIS_SINCE(forward_sample_start) > 2000)
									{
										cant_advance_forward = false;
									}

									if (GET_MILLIS_SINCE(backward_sample_start) > 1000)
									{
										if (backward_sample_start != 0)
										{
											cant_advance_backwards = (backward_sample_pos.distance(veh_pos) < 0.5f);
										}
										backward_sample_start = get_current_time_millis();
										backward_sample_pos = veh_pos;
									}
								}
							}
							else
							{
								TASK::TASK_VEHICLE_TEMP_ACTION(driver, veh, 27, 60);
								if (veh_pos.distance(player_pos) > veh_size)
								{
									++stopping;
								}
								else
								{
									if (stopping != 0)
									{
										++stopping;
									}
								}
							}
						}
					}
					return true;
				}
				cleanup();
				return false;
			});
		}
	};
}
