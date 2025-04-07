#include "CommandListTextSettings.hpp"

#include "dx_common.hpp"

#include "CommandboxGrid.hpp"
#include "CommandLambdaSlider.hpp"
#include "CommandSlider.hpp"
#include "Gui.hpp"
#include "NotifyGrid.hpp"
#include "TextSettings.hpp"

namespace Stand
{
	CommandListTextSettings::CommandListTextSettings(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, TextSettings* settings)
		: CommandList(parent, std::move(menu_name), std::move(command_names))
	{
		this->createChild<CommandLambdaSlider<CommandSlider>>(LOC("SCLE"), getSuffixedCommandNames(CMDNAME("scale")), NOLABEL, 1, SHRT_MAX, int(settings->scale / float(TEXT_HEIGHT_PX) / 2.0f), 1, [settings](int value, Click& click)
		{
			if (g_gui.canUpdateThemeSettingWithDefaultDependency())
			{
				settings->scale = (float(value) * float(TEXT_HEIGHT_PX) * 2.0f);
				g_notify_grid.update();
				g_commandbox_grid.update();
			}
		});
		this->createChild<CommandLambdaSlider<CommandSlider>>(LOC("OFF_X"), getSuffixedCommandNames(CMDNAME("xoffset")), NOLABEL, -15, 15, (int)settings->x_offset, 1, [settings](int value, Click& click)
		{
			if (g_gui.canUpdateThemeSettingWithDefaultDependency())
			{
				settings->x_offset = (float)value;
			}
		});
		this->createChild<CommandLambdaSlider<CommandSlider>>(LOC("OFF_Y"), getSuffixedCommandNames(CMDNAME("yoffset")), NOLABEL, -10, 10, (int)settings->y_offset, 1, [settings](int value, Click& click)
		{
			if (g_gui.canUpdateThemeSettingWithDefaultDependency())
			{
				settings->y_offset = (float)value;
			}
		});
	}
}
