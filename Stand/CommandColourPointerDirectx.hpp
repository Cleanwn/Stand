#pragma once

#include "CommandColour.hpp"

#include "Gui.hpp"

namespace Stand
{
	class CommandColourPointerDirectx : public CommandColour
	{
	private:
		DirectX::SimpleMath::Color* const colour;

	public:
		explicit CommandColourPointerDirectx(CommandList* const parent, DirectX::SimpleMath::Color* const colour, Label&& menu_name, std::vector<CommandName>&& command_names, CommandColourCustom* with_copy_from = nullptr)
			: CommandColour(parent, std::move(menu_name), std::move(command_names), NOLABEL, *colour, true, CMDFLAGS_COLOUR, with_copy_from), colour(colour)
		{
		}

		void onChange(Click& click) final
		{
			if (g_gui.canUpdateThemeSettingWithDefaultDependency())
			{
				*colour = getRGBA();
			}
		}
	};
}
