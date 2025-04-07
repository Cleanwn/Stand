#pragma once

#include "CommandGamerPresets.hpp"

namespace Stand
{
	class CommandGamer : public CommandGamerPresets
	{
	public:
		CommandInputRid* rid;
	private:
		CommandInputGamerName* name;

	public:
		explicit CommandGamer(CommandList* parent, std::string&& menu_name, const std::vector<CommandName>& command_names, bool include_rockstar_admins);

		[[nodiscard]] int64_t getRid() const noexcept;

		void onPresetClick(Click& click, int64_t rid, const char* name) final;
		void setRid(int64_t rid);
		void onRidUpdate();
	};
}
