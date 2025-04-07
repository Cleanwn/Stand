#pragma once

#include <string>

#include "CommandListFolder.hpp"

namespace Stand
{
	struct CommandListFolderItemData
	{
		std::wstring rel_path;
	};

	template <typename Base, typename This>
	class CommandListFolderItem : public Base
	{
	protected:
		using Base::Base;

	public:
		[[nodiscard]] __forceinline std::wstring getRelativeFilePath() const
		{
			return ((This*)this)->folder_item_data.rel_path;
		}

		[[nodiscard]] CommandListFolder* getCommandListFolder()
		{
			return (CommandListFolder*)((Command*)this)->resolveParent(COMMAND_LIST_REFRESHABLE);
		}

		[[nodiscard]] const CommandListFolder* getCommandListFolder() const
		{
			return (const CommandListFolder*)((const Command*)this)->resolveParent(COMMAND_LIST_REFRESHABLE);
		}

		[[nodiscard]] std::wstring getFilePath() const
		{
			return getCommandListFolder()->getFilePath(&((This*)this)->folder_item_data);
		}
	};
}
