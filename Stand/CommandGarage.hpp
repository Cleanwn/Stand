#pragma once

#include "CommandListFolder.hpp"

namespace Stand
{
	class CommandGarage : public CommandListFolder
	{
	public:
		explicit CommandGarage(CommandList* parent);

	protected:
		void populateHead() final;

		[[nodiscard]] CommandListFolderItemData* getItemData(Command* cmd) const noexcept final;

		void onItemIsKept(Command* cmd) const final;

		void createItem(CommandList* list, std::wstring&& filename_no_ext, CommandListFolderItemData&& data) const final;
	};
}
