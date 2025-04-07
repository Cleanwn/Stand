#pragma once

#include "CommandListNameshare.hpp"

namespace Stand
{
	class CommandListWeapons : public CommandListNameshare
	{
	protected:
		using CommandListNameshare::CommandListNameshare;

	public:
		void populate();

		[[nodiscard]] static CommandList* getHost(Command* cmd);

	protected:
		virtual void createCommand(CommandList* list, Label&& menu_name, std::string&& config_name, std::vector<hash_t>&& weapons) = 0;
	};
}
