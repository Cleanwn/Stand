#include "CommandProfile.hpp"

#include "CommandProfileSave.hpp"
#include "CommandProfileLoad.hpp"
#include "CommandProfileDelete.hpp"
#include "CommandProfileAutoload.hpp"
#include "CommandProfileAutosave.hpp"

#include "StringUtils.hpp"

namespace Stand
{
	CommandProfile::CommandProfile(CommandList* const parent, std::wstring&& name, CommandListFolderItemData&& folder_item_data)
		: CommandListFolderItem(parent, LIT(std::move(name)), {}, NOLABEL, CMDFLAGS_LIST | CMDFLAG_FEATURELIST_SKIP), folder_item_data(std::move(folder_item_data))
	{
		indicator_type = LISTINDICATOR_OFF;

		this->createChild<CommandProfileSave>();
		this->createChild<CommandProfileLoad>();
		this->createChild<CommandProfileAutoload>();
		this->createChild<CommandProfileAutosave>();
		this->createChild<CommandProfileDelete>();
	}

	std::string CommandProfile::getProfileNameA() const
	{
		return StringUtils::utf16_to_utf8(getProfileNameW());
	}

	std::wstring CommandProfile::getProfileNameW() const
	{
		auto name = getRelativeFilePath();
		name.erase(name.size() - 4, 4); // ".txt"
		return name;
	}

	std::wstring CommandProfile::getConfigName() const
	{
		auto name = getProfileNameW();
		name.insert(0, L"Profiles\\");
		return name;
	}

	bool CommandProfile::isActive() const
	{
		return g_gui.active_profile.getName() == getConfigName();
	}

	void CommandProfile::setActive() const
	{
		g_gui.active_profile.setName(getConfigName());
		g_gui.loadStateToMemory(g_gui.active_profile);
	}

	void CommandProfile::onTickInGameViewport()
	{
		if (isActive())
		{
			setHelpText(LOC("PRFL_H_ACTV"));
			setIndicatorType(LISTINDICATOR_ON);
		}
		else
		{
			setHelpText(NOLABEL);
			setIndicatorType(LISTINDICATOR_OFF);
		}
	}

	void CommandProfile::onTickInWebViewport()
	{
		return onTickInWebViewportImplRedirect();
	}
}
