#pragma once

#include "CommandList.hpp"

namespace Stand
{
	enum TabType : uint8_t
	{
		TAB_NONE = 0,
		TAB_SELF,
		TAB_VEHICLE,
		TAB_ONLINE,
		TAB_PLAYERS,
		TAB_WORLD,
		TAB_GAME,
		TAB_STAND,
	};

#define TABRENDER_LICON		0b1
#define TABRENDER_NAME		0b10
#define TABRENDER_RICON		0b100

	class CommandTab : public CommandList
	{
	public:
		inline static uint8_t render_mode = TABRENDER_NAME;

		const TabType tab_type;
		std::wstring render_name{};

		explicit CommandTab(CommandList* const root, const TabType tab_type, Label&& menu_name, std::vector<CommandName>&& command_names = {});

		void preDetach() final;

		std::wstring getCommandSyntax() const final;

		void updateRenderName();
	};
}
