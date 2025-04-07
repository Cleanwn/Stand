#pragma once

#include "CommandListFolder.hpp"

namespace Stand
{
	class CommandScripts final : public CommandListFolder
	{
	public:
		inline static CommandScripts* instance = nullptr;

		explicit CommandScripts(CommandList* parent);
		~CommandScripts() final;

	protected:
		void populateHead() final;

		void postPopulateBody() const final;

		[[nodiscard]] CommandListFolderItemData* getItemData(Command* cmd) const noexcept final;

		void onItemIsKept(Command* cmd) const final;

		void createItem(CommandList* list, std::wstring&& filename_no_ext, CommandListFolderItemData&& data) const final;
	};
}
