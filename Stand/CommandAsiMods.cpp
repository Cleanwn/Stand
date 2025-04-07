#include "CommandAsiMods.hpp"

#include <filesystem>

#include "asi_decl.hpp"
#include "CommandAsiMod.hpp"
#include "CommandLambdaAction.hpp"
#include "CommandTogglePointer.hpp"
#include "get_appdata_path.hpp"
#include "Gui.hpp"
#include "Label.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	CommandAsiMods::CommandAsiMods(CommandList* parent)
		: CommandListRefreshable(parent, LOC("ASI"), CMDNAMES("asimods"), LOC("ASI_H"))
	{
		instance = this;

		CommandPhysical::collapse_command_names.emplace(L"asi");
	}

	void CommandAsiMods::populateHead()
	{
		createChild<CommandLambdaAction>(LOC("OPNASI"), CMDNAMES(), LOC("OPNASI_H"), [](Click& click)
		{
			g_gui.shellExecute(get_appdata_path("ASI Mods\\").c_str());
		});
		createChild<CommandLambdaAction>(LOC("OPNGTA"), CMDNAMES(), NOLABEL, [](Click& click)
		{
			g_gui.shellExecute(std::filesystem::current_path().wstring().c_str());
		});
		// Settings
		{
			auto settings = createChild<CommandList>(LOC("STTNGS"));
			settings->createChild<CommandTogglePointer>(&AsiMod::use_temp_dir, LOC("ASI_TMP"), {}, LOC("ASI_TMP_H"));
			settings->createChild<CommandTogglePointer>(&AsiSettings::deceive_online_checks, LOC("ASI_DOC"), {}, LOC("ASI_DOC_H"));
		}
	}

	void CommandAsiMods::populateBody()
	{
		const auto stand_asi_directory = std::move(std::wstring(_wgetenv(L"appdata")).append(LR"(\Stand\ASI Mods\)"));
		if (!std::filesystem::exists(stand_asi_directory))
		{
			std::filesystem::create_directory(stand_asi_directory);
		}
		processAsiDirectory(stand_asi_directory, ASIDIR_STAND);
		auto game_dir = std::filesystem::current_path().wstring();
		game_dir.push_back(L'\\');
		processAsiDirectory(game_dir, ASIDIR_GAME);
	}

	void CommandAsiMods::processAsiDirectory(const std::wstring& path, const AsiDirectory dir)
	{
		std::error_code ec{};
		for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(path, ec))
		{
			if (entry.is_regular_file())
			{
				std::wstring filename = entry.path().wstring().substr(path.length());
				if (filename.length() > 4)
				{
					auto ext = filename.substr(filename.length() - 4);
					StringUtils::to_lower(ext);
					if (ext == L".asi")
					{
						auto name = StringUtils::utf16_to_utf8(filename.substr(0, filename.length() - 4));
						if (!hasAsiMod(name, dir))
						{
							createChild<CommandAsiMod>(LIT(std::move(name)), dir);
						}
					}
				}
			}
		}
	}

	[[nodiscard]] static AsiMod* getAsiMod(Command* cmd)
	{
		return cmd->as<CommandAsiMod>()->asi.get();
	}

	bool CommandAsiMods::hasAsiMod(const std::string& name, const AsiDirectory dir) const
	{
		for (auto i = children.begin() + getFullBodyOffset(); i != children.end(); ++i)
		{
			auto* cmd = (CommandPhysical*)i->get();
			if (getAsiMod(cmd)->dir == dir && cmd->menu_name.getLiteralUtf8() == name)
			{
				return true;
			}
		}
		return false;
	}

	std::vector<AsiMod*> CommandAsiMods::getAllAsiMods() const
	{
		std::vector<AsiMod*> res{};
		for (auto i = children.begin() + getFullBodyOffset(); i != children.end(); ++i)
		{
			res.emplace_back(getAsiMod(i->get()));
		}
		return res;
	}

	AsiMod* CommandAsiMods::getAsiModByModule(HANDLE hmod) const
	{
		for (auto i = children.begin() + getFullBodyOffset(); i != children.end(); ++i)
		{
			auto* asi = getAsiMod(i->get());
			if (asi->asi_h == hmod)
			{
				return asi;
			}
		}
		return nullptr;
	}

	bool CommandAsiMods::shouldDelete(Command* cmd) const
	{
		return !std::filesystem::is_regular_file(getAsiMod(cmd)->getPath());
	}
}
