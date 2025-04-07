#include "CommandGamer.hpp"

#include <fmt/format.h>

#include "CommandInputGamerName.hpp"
#include "CommandGamerRid.hpp"
#include "CommandList.hpp"

namespace Stand
{
	CommandGamer::CommandGamer(CommandList* parent, std::string&& menu_name, const std::vector<CommandName>& command_names, bool include_rockstar_admins)
		: CommandGamerPresets(parent, LIT(std::move(std::string(menu_name).append(": ").append(LANG_GET("PRSTS")))), include_rockstar_admins)
	{
		rid = parent->createChild<CommandGamerRid>(menu_name, command_names, this);
		name = parent->createChild<CommandInputGamerName>(std::move(menu_name), command_names, this);
	}

	int64_t CommandGamer::getRid() const noexcept
	{
		return rid->getRid();
	}

	void CommandGamer::onPresetClick(Click& click, int64_t rid, const char* name)
	{
		setRid(rid);
	}

	void CommandGamer::setRid(int64_t rid)
	{
		this->rid->setValue(fmt::to_string(rid));
		onRidUpdate();
	}

	void CommandGamer::onRidUpdate()
	{
		this->name->reset();
	}
}
