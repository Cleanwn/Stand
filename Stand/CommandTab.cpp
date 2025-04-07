#include "CommandTab.hpp"

#include "Renderer.hpp"
#include "UnicodePrivateUse.hpp"

namespace Stand
{
	CommandTab::CommandTab(CommandList* const root, const TabType tab_type, Label&& menu_name, std::vector<CommandName>&& command_names)
		: CommandList(root, std::move(menu_name), std::move(command_names), NOLABEL, CMDFLAGS_LIST | (tab_type == TAB_NONE ? CMDFLAG_TEMPORARY : 0)), tab_type(tab_type)
	{
		updateRenderName();
	}

	// Specialisation of CommandList::preDetach
	void CommandTab::preDetach()
	{
		CommandPhysical::preDetach();

		if (!g_renderer.areTabsEnabled())
		{
			evacuateCursor();
		}
	}

	std::wstring CommandTab::getCommandSyntax() const
	{
		if (!this->command_names.empty())
		{
			return getCommandSyntaxImpl(std::move(std::wstring(L"t ").append(cmdNameToUtf16(this->command_names.at(0).substr(1)))));
		}
		return {};
	}

	void CommandTab::updateRenderName()
	{
		std::wstring render_name{};
		if ((render_mode & TABRENDER_LICON) && tab_type != NONE)
		{
			render_name.push_back(L'\uE700' + tab_type);
		}
		if (render_mode & TABRENDER_NAME)
		{
			if (!render_name.empty())
			{
				render_name.push_back(L' ');
			}
			render_name.append(menu_name.getLocalisedUtf16());
		}
		if ((render_mode & TABRENDER_RICON) && tab_type != NONE)
		{
			if (!render_name.empty())
			{
				render_name.push_back(L' ');
			}
			render_name.push_back(L'\uE700' + tab_type);
		}
		this->render_name = std::move(render_name);
	}
}
