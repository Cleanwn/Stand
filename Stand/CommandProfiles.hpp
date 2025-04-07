#pragma once

#include "CommandListFolder.hpp"

namespace Stand
{
	class CommandProfiles : public CommandListFolder
	{
	public:
		explicit CommandProfiles(CommandList* const parent);

		void onClick(Click& click) final;

		void populateHead() final;

		CommandListFolderItemData* getItemData(Command* cmd) const noexcept final;

		void createItem(CommandList* list, std::wstring&& filename_no_ext, CommandListFolderItemData&& data) const final;
	};
}
