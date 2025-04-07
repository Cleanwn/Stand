#pragma once

#include "CommandHandlingFlag.hpp"

#include "atoffset.hpp"
#include "CHandlingData.hpp"

namespace Stand
{
	class CommandDriftMode : public CommandHandlingFlag<CCarHandlingData>
	{
	private:
		std::unordered_map<uintptr_t, float> old_values;

		static const uint32_t flags = 1 << 4
			| 1 << 6
			| 1 << 8
			| 1 << 9
			// | 1 << 10 Disallows exiting first gear, possibly CF_GEARBOX_MANUAL. This is set on the DRIFT vehicles? The game might be manually using a different gear shift algorithm.
			| 1 << 13 // Causes us to lose a lot more traction and spin out constantly, possibly CF_ASSIST_TRACTION_CONTROL but a disable-toggle instead of an enable.
			| 1 << 15
			// | 1 << 16 Causes the vehicle to shift before nearing redline RPM.
			| 1 << 18
			| 1 << 20
			| 1 << 22
			// | 1 << 23 Causes vehicles to move and shift/accelerate whenever steering is applied. On drift vehicles, this gives more control to the front wheels.
			;

		void setHandlingValue(Offset<CHandlingData, float> offset, CHandlingData* data, float value)
		{
			old_values.emplace(offset.offset, offset.resolve(*data));
			offset.resolve(*data) = value;
		}

		[[nodiscard]] static std::pair<float, float> getTractionValues(AbstractEntity& vehicle)
		{
			const bool in_snow = MISC::GET_SNOW_LEVEL() != 0.0f;

			float base_curve_min = !in_snow ? 1.625f : 1.925f;
			float base_curve_max = !in_snow ? 1.63f : 1.93f;

			const float speed_mps = ENTITY::GET_ENTITY_SPEED(vehicle);

			if (speed_mps < 4.5f)
			{
				base_curve_min *= 1.50f;
				base_curve_max *= 1.50f;
			}
			else if (speed_mps < 8.0f)
			{
				base_curve_min *= 1.30f;
				base_curve_max *= 1.30f;
			}
			else if (speed_mps < 13.0f)
			{
				base_curve_min *= 1.15f;
				base_curve_max *= 1.15f;
			}

			return std::make_pair(base_curve_min, base_curve_max);
		}

	public:
		explicit CommandDriftMode(CommandList* const parent)
			: CommandHandlingFlag<CCarHandlingData>(parent, LOC("DRFTMDE"), CMDNAMES_OBF("driftmode"), LOC("DRFTMDE_H"), &CCarHandlingData::aFlags, flags, true)
		{
		}

	protected:
		void doChangeExtra(AbstractEntity& vehicle) override
		{
			ensureScriptThread([=]() mutable
			{
				SOUP_IF_LIKELY (auto cveh = vehicle.getCVehicle())
				{
					SOUP_IF_LIKELY (auto handling = cveh->handling_data)
					{
						const auto tractions = getTractionValues(vehicle);

						setHandlingValue(&CHandlingData::m_acceleration, handling, 1.65f);
						setHandlingValue(&CHandlingData::m_suspension_force, handling, 2.0f);
						setHandlingValue(&CHandlingData::m_suspension_raise, handling, 0.025f);
						setHandlingValue(&CHandlingData::m_suspension_comp_damp, handling, 0.1f);
						setHandlingValue(&CHandlingData::m_traction_loss_mult, handling, 1.0f);
						setHandlingValue(&CHandlingData::m_upshift, handling, 4.8f);
						setHandlingValue(&CHandlingData::m_downshift, handling, 4.7f);
						setHandlingValue(&CHandlingData::m_traction_curve_min, handling, tractions.first);
						setHandlingValue(&CHandlingData::m_traction_curve_max, handling, tractions.second);
						setHandlingValue(&CHandlingData::m_drive_bias_rear, handling, 1.1764f);
						setHandlingValue(&CHandlingData::m_drive_bias_front, handling, 0.35f);
						setHandlingValue(&CHandlingData::m_drive_inertia, handling, 1.0f);
						setHandlingValue(&CHandlingData::m_initial_drive_force, handling, 0.7059f);
						setHandlingValue(&CHandlingData::m_drive_max_flat_velocity, handling, 44.1666f);
						setHandlingValue(&CHandlingData::m_initial_drive_max_flat_vel, handling, 36.8055f);
						setHandlingValue(&CHandlingData::m_brake_force, handling, 0.8750f);
						setHandlingValue(&CHandlingData::m_brake_bias_front, handling, 0.64f);
						setHandlingValue(&CHandlingData::m_brake_bias_rear, handling, 1.36f);
						setHandlingValue(&CHandlingData::m_handbrake_force, handling, 0.9f);
						setHandlingValue(&CHandlingData::m_steering_lock, handling, 1.3482f);
						setHandlingValue(&CHandlingData::m_steering_lock_ratio, handling, 0.7416f);
						setHandlingValue(&CHandlingData::m_traction_curve_lateral, handling, 0.6535f);
						setHandlingValue(&CHandlingData::m_curve_lateral, handling, 0.5139f);
						setHandlingValue(&CHandlingData::m_curve_lateral_ratio, handling, 1.9455f);
						setHandlingValue(&CHandlingData::m_traction_spring_delta_max, handling, 0.1500f);
						setHandlingValue(&CHandlingData::m_traction_spring_delta_max_ratio, handling, 6.6666f);
						setHandlingValue(&CHandlingData::m_low_speed_traction_loss_mult, handling, 0.0f);
						setHandlingValue(&CHandlingData::m_camber_stiffness, handling, 0.0f);
						setHandlingValue(&CHandlingData::m_traction_bias_front, handling, 0.9240f);
						setHandlingValue(&CHandlingData::m_traction_bias_rear, handling, 1.076f);
						setHandlingValue(&CHandlingData::m_suspension_rebound_damp, handling, 0.155f);
						setHandlingValue(&CHandlingData::m_suspension_upper_limit, handling, 0.0675f);
						setHandlingValue(&CHandlingData::m_suspension_lower_limit, handling, -0.1f);
						setHandlingValue(&CHandlingData::m_suspension_bias_front, handling, 1.04f);
						setHandlingValue(&CHandlingData::m_suspension_bias_rear, handling, 0.96f);

						VEHICLE::MODIFY_VEHICLE_TOP_SPEED(vehicle, 30.0f);
					}
				}			
			});
		}

		void undoChangeExtra(AbstractEntity& vehicle) override
		{
			SOUP_IF_LIKELY (auto cveh = vehicle.getCVehicle())
			{
				SOUP_IF_LIKELY (auto handling = cveh->handling_data)
				{
					for (auto [offset, old_value] : old_values)
					{
						*atoffset<float>(handling, offset) = old_value;
					}

					VEHICLE::MODIFY_VEHICLE_TOP_SPEED(vehicle, 10.0f);
				}
			}

			old_values.clear();
		}

		bool requiresChange(AbstractEntity& vehicle) override final
		{
			const auto model = vehicle.getModel();

			if (!model.isBike()
				&& !model.isBicycle()
				&& !model.isAquatic()
				&& !model.isAirborne()
				&& !model.hasDriftTune()
				)
			{
				return true;
			}

			return false;
		}
	};
}