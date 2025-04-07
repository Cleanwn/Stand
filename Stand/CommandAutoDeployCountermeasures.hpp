#pragma once

#include "CommandListSelect.hpp"

#include "eHomingLockOnState.hpp"
#include "CommandFireCountermeasure.hpp"
#include "gta_vehicle.hpp"

namespace Stand
{
	class CommandAutoDeployCountermeasures : public CommandListSelect
	{
	private:
		using CountermeasureType = CommandFireCountermeasure::CountermeasureType;

		time_t last_flare_deployment = 0;
		time_t last_periodic_deployment = 0;

		CommandToggle* periodic = nullptr;
		CommandSlider* periodic_delay = nullptr;
		CommandFireCountermeasure* command = nullptr;

		[[nodiscard]] CommandFireCountermeasure* getCommand()
		{
			if (!this->command)
			{
				this->command = this->parent->children.at(2)->as<CommandFireCountermeasure>();
			}

			return this->command;
		};

		[[nodiscard]] static std::vector<CommandListActionItemData> getOptions()
		{
			return std::vector<CommandListActionItemData>{
				CommandListActionItemData(CountermeasureType::NONE, LOC("DOFF")),
				CommandListActionItemData(CountermeasureType::FLARE, LOC("FLARE")),
				CommandListActionItemData(CountermeasureType::CHAFF, LOC("CHAFF")),
				CommandListActionItemData(CountermeasureType::BOTH, LOC("BOTH")),
			};
		}

		[[nodiscard]] bool isMissileIncoming() const
		{
			if (auto cveh = g_player_veh.getCVehicle())
			{
				return cveh->m_fHomingProjectileDistance != -1.0f && cveh->m_fHomingProjectileDistance <= 200.0f;
			}

			return false;
		}

	public:
		explicit CommandAutoDeployCountermeasures(CommandList* const parent, CommandToggle* const periodic, CommandSlider* const periodic_delay)
			: CommandListSelect(parent, LOC("CM_AUTO"), CMDNAMES_OBF("autodeploy"), NOLABEL, getOptions(), CountermeasureType::NONE), periodic(periodic), periodic_delay(periodic_delay)
		{
		}

		void onChange(Click& click, long long prev_value) final
		{
			const auto preference = (CountermeasureType)this->value;

			if (preference == CountermeasureType::NONE)
			{
				return;
			}

			registerScriptTickEventHandler([this, preference]
			{
				HANDLER_CHECK(preference == this->value);

				if (g_player_veh.isValid()
					&& getCommand()->shouldDeploy()
					&& g_player_veh.isOwnerOfVehicleAndDriver()
					)
				{
					if (periodic->m_on
						&& GET_MILLIS_SINCE(last_periodic_deployment) >= periodic_delay->value * 1000
						)
					{
						last_periodic_deployment = get_current_time_millis();
						getCommand()->deployRelevant(preference);
						HANDLER_END;
					}

					if (preference != CountermeasureType::FLARE
						&& VEHICLE::GET_VEHICLE_HOMING_LOCKEDONTO_STATE((Any)g_player_veh.getHandle()) != eHomingLockOnState::HLOnS_NONE
						)
					{
						getCommand()->deployChaff();
					}

					if (preference != CountermeasureType::CHAFF
						&& isMissileIncoming()
						&& GET_MILLIS_SINCE(last_flare_deployment) >= 2000
						)
					{
						getCommand()->deployFlares();
						last_flare_deployment = get_current_time_millis();
					}
				}

				HANDLER_END;
			});
		};
	};
}