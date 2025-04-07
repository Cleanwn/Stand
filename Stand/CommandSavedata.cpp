#ifdef STAND_DEBUG

#include "CommandSavedata.hpp"

#include <fstream>

#include <soup/json.hpp>
#include <soup/string.hpp>

#include "CommandLambdaAction.hpp"
#include "CommandToggle.hpp"
#include "CommandTogglePointer.hpp"
#include "ComponentSavedata.hpp"
#include "drawDebugText.hpp"
#include "get_appdata_path.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	class CommandSavedataShow : public CommandToggle
	{
	public:
		explicit CommandSavedataShow(CommandList* const parent)
			: CommandToggle(parent, LIT("Show"))
		{
		}

		void onChange(Click& click) final
		{
			onChangeToggleScriptTickEventHandler(click, []
			{
				drawDebugText(g_comp_savedata.obj.encodePretty());
			});
		}
	};

	CommandSavedata::CommandSavedata(CommandList* const parent)
		: CommandList(parent, LIT("Savedata"))
	{
		this->createChild<CommandTogglePointer>(&g_comp_savedata.record, LIT("Record"));
		this->createChild<CommandTogglePointer>(&g_comp_savedata.replay, LIT("Replay"));
		this->createChild<CommandSavedataShow>();
		this->createChild<CommandLambdaAction>(LIT("Export"), {}, NOLABEL, [](Click&)
		{
			std::ofstream of(get_appdata_path("Savedata.json"));
			of << g_comp_savedata.obj.encodePretty();
		});
		this->createChild<CommandLambdaAction>(LIT("Import"), {}, NOLABEL, [](Click&)
		{
			auto data = soup::string::fromFile(StringUtils::utf16_to_utf8(get_appdata_path("Savedata.json")));
			auto json = soup::json::decode(data);
			g_comp_savedata.obj = std::move(*reinterpret_cast<soup::JsonObject*>(json.release()));
		});
		this->createChild<CommandLambdaAction>(LIT("Clear"), {}, NOLABEL, [](Click&)
		{
			g_comp_savedata.obj.children.clear();
		});
	}
}

#endif
