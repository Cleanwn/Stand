#pragma once

#include "CommandOnCurrentVehicle.hpp"

#include "AbstractModel.hpp"
#include "CHandlingData.hpp"
#include "gta_vehicle.hpp"

namespace Stand
{
	class CommandDisableHeliAutoStabilisation : public CommandOnCurrentVehicleToggle
	{
	private:
		struct Values
		{
			float m_Thrust;
			float m_AttackLiftMult;
			float m_AttackDiveMult;
			float m_PitchStabilise;
			float m_RollStabilise;
			float m_YawStabilise;
			float m_WindMult;
		};

		std::unordered_map<Hash, Values> og;

		[[nodiscard]] CFlyingHandlingData* getHeliHandlingData(AbstractEntity& veh)
		{
			if (veh.getModel().isHeli())
			{
				SOUP_IF_LIKELY (auto cveh = veh.getCVehicle())
				{
					SOUP_IF_LIKELY (auto handling = cveh->handling_data)
					{
						return (CFlyingHandlingData*)handling->getSubhandlingData(eHandlingType::HANDLING_TYPE_FLYING);
					}
				}
			}

			return nullptr;
		}

	public:
		explicit CommandDisableHeliAutoStabilisation(CommandList* const parent)
			: CommandOnCurrentVehicleToggle(parent, LOC("HELIAUTOSTAB"), CMDNAMES("heliautostabilisation", "disableheliautostabilisation"), LOC("HELIAUTOSTAB_H"))
		{
		}

	protected:
		void doChangeImpl(AbstractEntity& veh) final
		{
			SOUP_IF_LIKELY (auto data = getHeliHandlingData(veh))
			{
				if (const auto hash = veh.getModelHash(); !og.contains(hash))
				{
					og.emplace(hash, Values{
						data->m_Thrust,
						data->m_AttackLiftMult,
						data->m_AttackDiveMult,
						data->m_PitchStabilise,
						data->m_RollStabilise,
						data->m_YawStabilise,
						data->m_WindMult
					});

					data->m_Thrust *= 1.35f;
				}

				data->m_AttackLiftMult = 0.0f;
				data->m_AttackDiveMult = 0.0f;
				data->m_PitchStabilise *= 0.01f;
				data->m_RollStabilise *= 0.01f;
				data->m_YawStabilise *= 0.01f;
				data->m_WindMult = 0.0f;
			}
		}

		void undoChangeImpl(AbstractEntity& veh) final
		{
			SOUP_IF_LIKELY (auto data = getHeliHandlingData(veh))
			{
				SOUP_IF_LIKELY (auto og_values = og.find(veh.getModelHash()); og_values != og.end())
				{
					data->m_Thrust = og_values->second.m_Thrust;
					data->m_AttackLiftMult = og_values->second.m_AttackLiftMult;
					data->m_AttackDiveMult = og_values->second.m_AttackDiveMult;
					data->m_PitchStabilise = og_values->second.m_PitchStabilise;
					data->m_RollStabilise = og_values->second.m_RollStabilise;
					data->m_YawStabilise = og_values->second.m_YawStabilise;
					data->m_WindMult = og_values->second.m_WindMult;

					og.erase(og_values);
				}
			}
		}

		bool requiresChange(AbstractEntity& veh) override final
		{
			return true;
		}
	};
}