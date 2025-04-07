#include "CommandListTransform.hpp"

#include "CommandDivider.hpp"
#include "CommandFindModel.hpp"
#include "CommandTransformModel.hpp"
#include "PedModel.hpp"
#include "CommandListTransform.hpp"

namespace Stand
{
	CommandListTransform::CommandListTransform(CommandList* const parent)
		: CommandList(parent, LOC("CHNGMDL"))
	{
		this->createChild<CommandFindModel>();
		hash_t category_name = 0;
		CommandList* category_list;
		for (const auto& model : PedModel::all)
		{
			if (category_name != model.category)
			{
				category_name = model.category;
				category_list = this->createChild<CommandList>(LOC_RT(category_name));
			}
			category_list->createChild<CommandTransformModel>(&model);
		}
	}
}
