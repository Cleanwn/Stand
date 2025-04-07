#pragma once

#include "CommandList.hpp"


namespace Stand
{
	class CommandListHistoricPlayers : public CommandList
	{
	private:
		bool was_active = false;

	public:
		explicit CommandListHistoricPlayers(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names = {}, Label&& help_text = NOLABEL, const commandflags_t flags = CMDFLAGS_LIST);

		void populate();

		void onActiveListUpdate() final;

	protected:
		[[nodiscard]] virtual std::vector<CommandHistoricPlayer*> collectPlayers() const = 0;
	};
}
