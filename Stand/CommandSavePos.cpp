#include "CommandSavePos.hpp"

#include <filesystem>
#include <fstream>

#include "ensure_folder.hpp"
#include "lang.hpp"

namespace Stand
{
	CommandSavePos::CommandSavePos(CommandList* parent, Label&& menu_name, std::vector<CommandName>&& command_names)
		: CommandAction(parent, std::move(menu_name), std::move(command_names))
	{
	}

	std::wstring CommandSavePos::getCommandSyntax() const
	{
		std::wstring syntax{ LANG_GET_W("CMD") };
		syntax.append(L": ").append(cmdNameToUtf16(command_names.at(0))).push_back(L' ');
		syntax.append(LANG_GET_W("ARGNME"));
		return syntax;
	}

	void CommandSavePos::onCommand(Click& click, std::wstring& args)
	{
		auto path = std::move(std::wstring(_wgetenv(L"appdata")).append(LR"(\Stand\Places)"));
		ensure_folder(path);
		path.push_back(L'\\');
		path.append(std::move(args)).append(L".txt");
		args.clear();
		std::ofstream file(std::move(path));
		file << getPos();
		SOUP_IF_UNLIKELY (file.bad())
		{
			click.setResponse(LOC("GENFAIL"));
		}
	}
}
