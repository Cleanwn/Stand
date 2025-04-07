#pragma once

#include "CommandListSelect.hpp"

#include "AbstractEntity.hpp"
#include "AbstractModel.hpp"
#include "natives.hpp"
#include "PedModel.hpp"
#include "PedType.hpp"
#include "Script.hpp"
#include "Util.hpp"

namespace Stand
{
	enum CommandListSelectPedModelsFlags : uint8_t
	{
		ONLY_HOSTILE = 0b1,
		NO_HIRES_PED = 0b10,
	};

	class CommandListSelectPedModels : public CommandListSelect
	{
	public:
		explicit CommandListSelectPedModels(CommandList* const parent, std::vector<CommandName>&& command_names = {}, Label&& help_text = NOLABEL, long long default_value = 0, uint8_t flags = 0)
			: CommandListSelect(parent, LOC("MDL"), std::move(command_names), std::move(help_text), {}, default_value)
		{
			std::vector<CommandListActionItemData> ped_options{};
			
			ped_options.emplace_back(CommandListActionItemData(
				ATSTRINGHASH("player"),
				LOC("PLY"),
				CMDNAMES("player"),
				NOLABEL,
				COMMANDPERM_USERONLY,
				CMDFLAG_FEATURELIST_FINISHLIST,
				LOC("PLYBLCHRS")
			));

			for (const auto& ped : PedModel::all)
			{
				auto model = AbstractModel(ped.hash);

				if (((flags & ONLY_HOSTILE) && !model.canBecomeHostile())
					|| ((flags & NO_HIRES_PED) && model.isHiResPed())
					)
				{
					continue;
				}

				ped_options.emplace_back(CommandListActionItemData(
					ped.hash,
					ped.menu_name,
					ped.command_names,
					NOLABEL,
					COMMANDPERM_USERONLY,
					CMDFLAG_FEATURELIST_FINISHLIST,
					LOC_RT(ped.category),
					true
				));
			}

			this->setOptions(std::move(ped_options));
		}

		[[nodiscard]] AbstractEntity createPed(const v3& pos, const PedType ped_type, AbstractEntity clone_from = AbstractEntity::invalid()) const
		{
			const auto value = (Hash)this->value;

			if (value == ATSTRINGHASH("player"))
			{
				if (clone_from.isValid())
				{
					const Ped ped = PED::CLONE_PED(clone_from, TRUE, TRUE, TRUE);
					SOUP_IF_LIKELY (ped != 0)
					{
						return AbstractEntity::get(ped);
					}
				}
				return AbstractEntity::invalid();
			}

			STREAMING::REQUEST_MODEL(value);
			while (!STREAMING::HAS_MODEL_LOADED(value))
			{
				Script::current()->yield();
			}

			return Util::createPed(ped_type, value, pos, 0);
		}
	};
}