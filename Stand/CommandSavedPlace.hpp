#pragma once

#include "CommandWithOnTickFocused.hpp"
#include "CommandListFolderItemData.hpp"
#include "CommandActionScript.hpp"

#include <optional>

#include "Vector2Plus.hpp"

namespace Stand
{
	class CommandSavedPlace : public CommandWithOnTickFocused<CommandListFolderItem<CommandActionScript, CommandSavedPlace>>
	{
	public:
		CommandListFolderItemData folder_item_data;
		std::optional<Vector2Plus> data{};

		explicit CommandSavedPlace(CommandList* parent, std::wstring&& place_name, CommandListFolderItemData&& folder_item_data);

	private:
		bool ensureCoord();

	public:
		void onTickFocused() final;

		void onClickScriptThread(Click& click) final;
	};
}
