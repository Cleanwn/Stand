#pragma once

#include "CommandListFolderItemData.hpp"
#include "CommandList.hpp"

namespace Stand
{
	class CommandProfile : public CommandListFolderItem<CommandList, CommandProfile>
	{
	public:
		CommandListFolderItemData folder_item_data;

		explicit CommandProfile(CommandList* const parent, std::wstring&& name, CommandListFolderItemData&& folder_item_data);

		[[nodiscard]] std::string getProfileNameA() const;
		[[nodiscard]] std::wstring getProfileNameW() const;
		[[nodiscard]] std::wstring getConfigName() const;
		[[nodiscard]] bool isActive() const;
		void setActive() const;

		void onTickInGameViewport() final;
		void onTickInWebViewport() final;
	};
}
