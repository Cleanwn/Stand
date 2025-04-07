#pragma once

#include "CommandListFolder.hpp"

namespace Stand
{
	class CommandLuaScripts : public CommandListFolder
	{
	public:
		explicit CommandLuaScripts(CommandList* parent);

	protected:
		void onClick(Click& click) final;

		void populateHead() final;

		[[nodiscard]] CommandListFolderItemData* getItemData(Command* cmd) const noexcept final;

		[[nodiscard]] bool canShowFolder(const std::wstring& folder_rel_path) const final;

		void createItem(CommandList* list, std::wstring&& filename_no_ext, CommandListFolderItemData&& data) const final;
	};
}
