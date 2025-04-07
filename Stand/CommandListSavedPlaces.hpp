#pragma once

#include "CommandListFolder.hpp"

namespace Stand
{
	class CommandListSavedPlaces : public CommandListFolder
	{
	public:
		explicit CommandListSavedPlaces(CommandList* parent);

	protected:
		[[nodiscard]] CommandListFolderItemData* getItemData(Command* cmd) const noexcept final;

		void onItemIsKept(Command* cmd) const final;

		void createItem(CommandList* list, std::wstring&& filename_no_ext, CommandListFolderItemData&& data) const final;
	};
}
