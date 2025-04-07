#pragma once

#include "CommandListFolder.hpp"

namespace Stand
{
	class CommandWardrobe : public CommandListFolder
	{
	public:
		explicit CommandWardrobe(CommandList* parent);

	protected:
		void populateHead() final;

		[[nodiscard]] CommandListFolderItemData* getItemData(Command* cmd) const noexcept final;

		void onItemIsKept(Command* cmd) const final;

		void createItem(CommandList* list, std::wstring&& filename_no_ext, CommandListFolderItemData&& data) const final;

		void sortImpl() final;
	};
}
