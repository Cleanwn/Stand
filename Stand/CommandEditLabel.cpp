#include "CommandEditLabel.hpp"

#include "CommandExtraInfo.hpp"
#include "CommandLabel.hpp"
#include "Gui.hpp"
#include "StringUtils.hpp"
#include "Util.hpp"

namespace Stand
{
	CommandEditLabel::CommandEditLabel(CommandList* const parent)
		: CommandAction(parent, LOC("EDIT"), CMDNAMES("edit"))
	{
		command_names.at(0).append(utf8ToCmdName(parent->as<CommandLabel>()->key));
		StringUtils::simplifyCommandName(command_names.at(0));
	}

	void CommandEditLabel::onClick(Click& click)
	{
		std::wstring prefill{ cmdNameToUtf16(command_names.at(0)) };
		prefill.push_back(L' ');
		prefill.append(StringUtils::utf8_to_utf16(g_hooking.label_replacements.at(parent->as<CommandLabel>()->label_hash)));
		click.showCommandBoxIfPossible(std::move(prefill));
	}

	void CommandEditLabel::onCommand(Click& click, std::wstring& args)
	{
		g_hooking.label_replacements[parent->as<CommandLabel>()->label_hash] = StringUtils::utf16_to_utf8(args);
		args.clear();
		((CommandLabel*)parent)->on_edit();
	}

	void CommandEditLabel::getExtraInfo(CommandExtraInfo& info, std::wstring& args)
	{
		CommandAction::getExtraInfo(info, args);
		info.colour_selector = true;
	}
}
