#pragma once

#include "CommandActionName.hpp"

#include <filesystem>
#include <fstream>

#include "ColonsAndTabs.hpp"
#include "CommandListSave.hpp"
#include "lang.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	class CommandListSaveAction : public CommandActionName
	{
	public:
		explicit CommandListSaveAction(CommandList* parent, Label&& menu_name, std::vector<CommandName>&& command_names)
			: CommandActionName(parent, std::move(menu_name), std::move(command_names))
		{
			mustHaveCommandName();
		}

		void onCommand(Click& click, std::wstring& args) final
		{
			if (!onCommandValidateFileName(click, args))
			{
				return;
			}
			auto path = ((CommandListSave*)parent)->getFolder();
			if (!std::filesystem::exists(path))
			{
				std::filesystem::create_directory(path);
			}
			path.append(std::move(args)).append(L".txt");
			args.clear();
			std::vector<std::string> data_order_en{};
			{
				auto data_order_raw = ((CommandListSave*)parent)->getDataOrderRaw();
				for (auto& key_raw : data_order_raw)
				{
					if (!((CommandListSave*)parent)->shouldOmitEntry(key_raw))
					{
						data_order_en.emplace_back(Lang::get_en(key_raw));
					}
				}
			}
			std::ofstream file_out(std::move(path));
			ColonsAndTabs::writeBySchema(file_out, ((CommandListSave*)parent)->getDataEn(), data_order_en);
			SOUP_IF_UNLIKELY (file_out.bad())
			{
				click.setResponse(LOC("GENFAIL"));
			}
			else
			{
				((CommandListSave*)parent)->onSaved();
			}
		}
	};
}
