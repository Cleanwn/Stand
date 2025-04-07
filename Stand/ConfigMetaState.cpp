#include "ConfigMetaState.hpp"

#include "ColonsAndTabs.hpp"
#include "Gui.hpp"

namespace Stand
{
	ConfigMetaState::ConfigMetaState()
		: Config(L"Meta State")
	{
	}

	void ConfigMetaState::loadImpl(std::unordered_map<std::string, std::string>&& data)
	{
		this->data = std::move(data);
	}

	void ConfigMetaState::saveToFileNow()
	{
		std::ofstream of(getPath());
		if (g_gui.user_understands_navigation)
		{
			of << "Finished Tutorial: Yes\n";
		}
		if (g_gui.user_understands_commands)
		{
			of << "Understands Commands: Yes\n";
		}
		if (g_gui.user_understands_sliders)
		{
			of << "Understands Sliders: Yes\n";
		}
		if (g_gui.profiles_tutorial == 4)
		{
			of << "Understands Profiles: Yes\n";
		}
		if (g_gui.user_understands_context_menu)
		{
			of << "Understands Context Menu: Yes\n";
		}
		if (g_gui.user_understands_controller_open_close)
		{
			of << "Understands Controller Open/Close: Yes\n";
		}
		if (g_gui.user_prompted_for_controller_disables)
		{
			of << "Prompted For Controller Disables: Yes\n";
		}
		if (g_gui.user_understands_command_chaining)
		{
			of << "Understands Command Chaining: Yes\n";
		}
		ColonsAndTabs::writeBySchema(of, data, {
			"Load On Inject",
			"Autosave",
#ifdef STAND_DEBUG
			"Developer",
#endif
			"DNS Conduit",
			"Packages Source",
		}, true);
	}
}
