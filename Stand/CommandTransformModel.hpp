#pragma once

#include "CommandWithEntityPreview.hpp"
#include "CommandAction.hpp"

#include "PedModel.hpp"
#include "PedType.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandTransformModel : public CommandWithEntityPreview<CommandAction>
	{
	private:
		const Hash model_hash;

	public:
		explicit CommandTransformModel(CommandList* const parent, const PedModel* model)
			: CommandWithEntityPreview(parent, Label(model->menu_name), std::vector<CommandName>(model->command_names), NOLABEL, CMDFLAGS_ACTION | CMDFLAG_SEARCH_FINISHLIST | CMDFLAG_FEATURELIST_FINISHLIST), model_hash(model->hash)
		{
		}

		void onClick(Click& click) final
		{
			ensureScriptThread(click, [model_hash{ this->model_hash }](Click& click)
			{
				Util::transform(click, model_hash);
			});
		}

	protected:
		[[nodiscard]] Hash getModelHash() final
		{
			return model_hash;
		}

		[[nodiscard]] AbstractEntity createPreviewEntity() final
		{
			return Util::createPed(PED_TYPE_MISSION, getModelHash(), {}, 0.0f, false);
		}
	};
}
