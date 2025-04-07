#pragma once

#include "CommandListSelect.hpp"

#include "AbstractEntity.hpp"
#include "CommandListActionItemData.hpp"
#include "CommandLightSignals.hpp"
#include "ExecCtx.hpp"
#include "get_current_time_millis.hpp"
#include "gta_vehicle.hpp"
#include "Gui.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandTurnSignal : public CommandListSelect
	{
	private:
		bool handler_on = false;

	public:
		CommandToggle* automatic = nullptr;

		enum : uint8_t
		{
			RIGHT,
			LEFT,
			HAZARD,
			OFF,
		};

		float last_turn_signal_heading = -1.0f;
		AbstractEntity last_modified_vehicle = AbstractEntity::invalid();
		
		explicit CommandTurnSignal(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names)
			: CommandListSelect(parent, std::move(menu_name), std::move(command_names), NOLABEL, std::vector<CommandListActionItemData>{
				{ OFF, LOC("OFF"), CMDNAMES_OBF("off") },
				{ LEFT, LOC("B_ML"), CMDNAMES_OBF("left") },
				{ RIGHT, LOC("B_MR"), CMDNAMES_OBF("right") },
				{ HAZARD, LOC("TRNSIG_HAZ"), CMDNAMES_OBF("hazard") },
			}, OFF, CMDFLAGS_LIST_SELECT | CMDFLAG_NO_SAVED_STATE)
		{
		}

		void onChange(Click& click, long long prev_value) final
		{
			auto vehicle = Util::getVehicle();

			if (vehicle.isValid())
			{
				vehicle.requestControl(ATSTRINGHASH("CommandTurnSignal"), [this](AbstractEntity& entity)
				{
					if (auto cveh = entity.getCVehicle())
					{
						cveh->m_nVehicleFlags.bHazardLights = value == HAZARD;
						cveh->m_nVehicleFlags.bLeftIndicator = value == LEFT;
						cveh->m_nVehicleFlags.bRightIndicator = value == RIGHT;

						if (value == LEFT || value == RIGHT)
						{
							last_turn_signal_heading = ENTITY::GET_ENTITY_HEADING(entity);
						}
						else
						{
							last_turn_signal_heading = -1.0f;
						}

						last_modified_vehicle = entity;
					}
				});

				if (!handler_on)
				{
					initialiseHandler();
				}
			}
			else
			{
				click.setResponse(LOC("NOVEHFND"));
				applyDefaultState();
			}
		}

	private:
		static void disableAll(AbstractEntity& vehicle)
		{
			if (vehicle.isValid() && vehicle.isOwner())
			{
				SOUP_IF_LIKELY(const auto cveh = vehicle.getCVehicle())
				{
					cveh->m_nVehicleFlags.bHazardLights = false;
					cveh->m_nVehicleFlags.bLeftIndicator = false;
					cveh->m_nVehicleFlags.bRightIndicator = false;
				}
			}
		}

		void initialiseHandler()
		{
			handler_on = true;

			registerScriptTickEventHandler([=]
			{
				auto vehicle = Util::getVehicle();

				if (vehicle.isValid()
					&& last_modified_vehicle.isValid()
					&& last_modified_vehicle != vehicle // Different vehicle?
					)
				{
					disableAll(last_modified_vehicle); // Cleanse the last one. 
					applyDefaultState();
					last_modified_vehicle.reset();
				}
				else if (!vehicle.isValid()) // If this is the case, we already checked for the last vehicle.
				{
					applyDefaultState();
					last_modified_vehicle.reset();
				}
				else if (automatic->m_on
					&& last_turn_signal_heading != -1.0f
					&& std::abs(ENTITY::GET_ENTITY_HEADING(vehicle) - last_turn_signal_heading) > 60
					)
				{
					applyDefaultState();
					last_modified_vehicle.reset();
					last_turn_signal_heading = -1.0f;
					disableAll(vehicle);
				}

				if (g_gui.isUnloadPending())
				{
					disableAll(vehicle);
					return false;
				}

				if (value == CommandTurnSignal::OFF)
				{
					handler_on = false;
					return false;
				}

				HANDLER_END;
			});
		}
	};
}