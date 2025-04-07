#include "CommandExportDisplayNames.hpp"

#include "get_display_names.hpp"
#include "FileLogger.hpp"

namespace Stand
{
	CommandExportDisplayNames::CommandExportDisplayNames(CommandList* parent)
		: CommandAction(parent, LIT("Export Display Names"))
	{
	}

	void CommandExportDisplayNames::onClick(Click& click)
	{
		auto display_names = get_display_names();
		g_logger.log(std::move(std::string("---- Start Of Exported Display Names ----")));
		g_logger.enterBlockMode();
		for (auto& display_name : display_names)
		{
			g_logger.log(std::move(display_name));
		}
		g_logger.leaveBlockMode();
		g_logger.log(std::move(std::string("---- End Of Exported Display Names ----")));
		click.setResponse(LIT("Your Log.txt just gained a lot of weight."));
	}
}
