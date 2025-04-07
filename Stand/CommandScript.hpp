#pragma once

#include "CommandAction.hpp"

#include <fstream>
#include <optional>

#include "CommandListFolderItemData.hpp"
#include "Gui.hpp"
#include "StringUtils.hpp"
#include "UserScript.hpp"

namespace Stand
{
	class CommandScript : public CommandListFolderItem<CommandAction, CommandScript>
	{
	public:
		std::optional<UserScript> data = std::nullopt;
		CommandListFolderItemData folder_item_data;

		explicit CommandScript(CommandList* parent, const std::wstring& filename_no_ext, CommandListFolderItemData&& folder_item_data)
			: CommandListFolderItem(parent, LIT(filename_no_ext), { CMDNAME("exec") }, NOLABEL, CMDFLAGS_ACTION | CMDFLAG_SEARCH_FINISHLIST | CMDFLAG_FEATURELIST_SKIP), folder_item_data(folder_item_data)
		{
#if COMPACT_COMMAND_NAMES
			StringUtils::simplify(command_names.at(0).append(StringUtils::utf16_to_utf8(filename_no_ext)));
#else
			StringUtils::simplify(command_names.at(0).append(filename_no_ext));
#endif
		}

	private:
		void ensureData()
		{
			if (!data.has_value())
			{
				UserScript user_script{};
				std::ifstream file_in(getFilePath());
				for (std::string line; std::getline(file_in, line); )
				{
					if (line.empty())
					{
						continue;
					}
					if (line.length() > 12)
					{
						std::string line_lower(line);
						StringUtils::to_lower(line_lower);
						if (line_lower.substr(0, 12) == "description:")
						{
							user_script.description = line.substr(12);
							if (user_script.description.at(0) == ' ')
							{
								user_script.description.erase(0, 1);
							}
							continue;
						}
					}

					user_script.commands.emplace_back(line);
				}
				setHelpText(LIT(user_script.description));
				data = std::move(user_script);
			}
		}

	public:
		void onTickInGameViewport() final
		{
			ensureData();
		}

		void onTickInWebViewport() final
		{
			return onTickInWebViewportImplRedirect();
		}

		void onClick(Click& click) final
		{
			data = std::nullopt;
			ensureData();
			ensureYieldableScriptThread(click, [this]
			{
				Click click(CLICK_SCRIPTED, TC_SCRIPT_YIELDABLE);
				for (const auto& command : data->commands)
				{
					g_gui.triggerCommands(command, click);
				}
			});
		}
	};
}
