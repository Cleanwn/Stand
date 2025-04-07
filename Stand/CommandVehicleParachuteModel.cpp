#include "CommandVehicleParachuteModel.hpp"

#include "AbstractEntity.hpp"
#include "FiberPool.hpp"
#include "natives.hpp"
#include "ParachuteModels.hpp"
#include "Script.hpp"

#include "gta_vehicle.hpp"
#include "CVehicleModelInfo.hpp"

#include <fmt/format.h>
#include "FileLogger.hpp"

namespace Stand
{
	[[nodiscard]] static std::vector<CommandListActionItemData> CommandVehicleParachuteModel_getOptions()
	{
		std::vector<CommandListActionItemData> options{};
		for (const auto& parachute_model : g_parachute_models)
		{
			for (const auto& variation : parachute_model.variations)
			{
				options.emplace_back((uint64_t(variation.idx) << 32) | uint64_t(uint32_t(parachute_model.model)), variation.name);
			}
		}
		return options;
	}

	CommandVehicleParachuteModel::CommandVehicleParachuteModel(CommandList* parent)
		: CommandListSelect(parent, LOC("VIS"), {}, LOC("PARA_H_VIS"), CommandVehicleParachuteModel_getOptions(), ATSTRINGHASH("imp_prop_impexp_para_s"))
	{
	}

	void CommandVehicleParachuteModel::onChange(Click& click, long long prev_value)
	{
		if (value == default_value)
		{
			apply();
		}
		else
		{
			auto value = this->value;
			registerScriptTickEventHandler(click, [this, value]
			{
				HANDLER(this->value == value, apply());
			});
		}
	}

	void CommandVehicleParachuteModel::apply() const
	{
		auto veh = (CAutomobile*)g_player_veh.getPointer();
		if (veh != nullptr)
		{
			hash_t model = (value & 0xFFFFFFFF);
			int idx = (value >> 32);

			if (veh->m_ParachuteModelHash == model)
			{
				if (veh->parachute_texture_variation == idx)
				{
					// no change in model or texture
				}
				else
				{
					// texture is changed
					VEHICLE::VEHICLE_SET_PARACHUTE_MODEL_TINT_INDEX(g_player_veh, idx);
					// note: native accounts for parachute being open and changes it immediately, hard to emulate "externally"
				}
			}
			else
			{
				// model is changed
				if (veh->parachute_model_unk_index == -1)
				{
					// parachute was not opened yet, we're free to change it
					veh->m_ParachuteModelHash = model;
					veh->parachute_texture_variation = idx;
				}
			}
		}
	}
}
