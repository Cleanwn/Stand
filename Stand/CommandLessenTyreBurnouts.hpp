#pragma once

#include "CommandOnCurrentVehicle.hpp"

#include "CHandlingData.hpp"
#include "gta_vehicle.hpp"

namespace Stand
{
	class CommandLessenTyreBurnouts : public CommandOnCurrentVehicleToggle
	{
	private:
		static inline float last_value = 1.0f; // Seems to be 1.0f on every vehicle but we'll do this just in case.

	public:
		explicit CommandLessenTyreBurnouts(CommandList* const parent)
			: CommandOnCurrentVehicleToggle(parent, LOC("LESSENBURN"), CMDNAMES_OBF("launchcontrol", "lessentyreburnouts", "lessenburnouts"))
		{
		}

	protected:
		void doChangeImpl(AbstractEntity& vehicle) final
		{
			SOUP_IF_LIKELY (auto cveh = vehicle.getCVehicle())
			{
				auto mult = &cveh->handling_data->m_low_speed_traction_loss_mult;

				if (*mult != 0.0f)
				{
					last_value = *mult;
				}

				*mult = 0.0f;
			}
		}

		void undoChangeImpl(AbstractEntity& last_vehicle) final
		{
			SOUP_IF_LIKELY (auto cveh = last_vehicle.getCVehicle())
			{
				SOUP_IF_LIKELY (auto handling = cveh->handling_data)
				{
					handling->m_low_speed_traction_loss_mult = last_value;
					last_value = 1.0f;
				}
			}
		}

		bool requiresChange(AbstractEntity& vehicle) override final
		{
			return true;
		}
	};
}