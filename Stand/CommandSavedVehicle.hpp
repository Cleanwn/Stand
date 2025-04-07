#pragma once

#include "CommandWithEntityPreview.hpp"
#include "CommandListFolderItemData.hpp"
#include "CommandAction.hpp"

#include "ColonsVehicleCustomisationIo.hpp"

namespace Stand
{
	class CommandSavedVehicle : public CommandWithEntityPreview<CommandListFolderItem<CommandAction, CommandSavedVehicle>>
	{
	public:
		CommandListFolderItemData folder_item_data;
		ColonsVehicleCustomisationIo data{};

		explicit CommandSavedVehicle(CommandList* parent, std::wstring&& name, CommandListFolderItemData&& folder_item_data);

	private:
		bool ensureData();

	public:
		void onClick(Click& click) final;

	protected:
		[[nodiscard]] Hash getModelHash() final;
		[[nodiscard]] AbstractEntity createPreviewEntity() final;
	};
}
