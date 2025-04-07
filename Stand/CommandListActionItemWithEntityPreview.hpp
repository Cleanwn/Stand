#pragma once

#include "CommandListActionItem.hpp"
#include "CommandWithEntityPreview.hpp"

#include "AbstractModel.hpp"
#include "PedType.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandListActionItemWithEntityPreview : public CommandWithEntityPreview<CommandListActionItem>
	{
	public:
		using CommandWithEntityPreview::CommandWithEntityPreview;

	protected:
		[[nodiscard]] Hash getModelHash() final
		{
			return (Hash)this->value;
		}

		[[nodiscard]] AbstractEntity createPreviewEntity() final
		{
			const auto hash = getModelHash();
			const auto model = AbstractModel(hash);

			if (model.isVehicle())
			{
				return Util::createVehicle(hash, {}, 0, false);
			}
			else if (model.isPed())
			{
				return Util::createPed(PedType::PED_TYPE_ANIMAL, hash, {}, 0, false);
			}
			else if (model.isObject())
			{
				return Util::createObject(hash, {}, false);
			}
			
			SOUP_ASSERT(false);
		}
	};
}
