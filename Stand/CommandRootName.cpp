#include "CommandRootName.hpp"

#include "Auth.hpp"
#include "conf.hpp"
#include "Gui.hpp"
#include "Label.hpp"
#include "MenuGrid.hpp"
#include "StringUtils.hpp"
#include "Renderer.hpp"
#include "RootNameMgr.hpp"

namespace Stand
{
	static void addOption(std::vector<CommandListActionItemData>& vec, long long id, const std::string& name, Label&& help_text = NOLABEL)
	{
		vec.emplace_back(CommandListActionItemData{ id, LIT(name), CMDNAMES(), std::move(help_text) });
	}

	[[nodiscard]] static std::vector<CommandListActionItemData> getOptions()
	{
		std::vector<CommandListActionItemData> ret{};
		ret.emplace_back(CommandListActionItemData{ RN_NULL, LOC("HIDE") });
		addOption(ret, RN_NAME_ONLY, soup::ObfusString(STAND_NAME));
		addOption(ret, RN_VERSION_ONLY, STAND_RN_VERSION);
		addOption(ret, RN_STOCK, RootNameMgr::stock_root_name);
		if (RootNameMgr::authed_root_name != nullptr && RootNameMgr::stock_root_name != RootNameMgr::authed_root_name)
		{
			addOption(ret, RN_AUTHED, RootNameMgr::authed_root_name);
		}
		if (g_auth.license_permissions != LICPERM_FREE)
		{
			if (g_auth.unlocks & UNLOCK_RNDBOOL)
			{
				long long i = RN_RNDBOOL_TRUE;
				for (const auto& root_name : RootNameMgr::rndbool_true)
				{
					addOption(ret, i, root_name, LOC("RN_RNDBOOL"));
					++i;
				}
			}
			else
			{
				long long i = RN_RNDBOOL_FALSE;
				for (const auto& root_name : RootNameMgr::rndbool_false)
				{
					addOption(ret, i, root_name, LOC("RN_RNDBOOL"));
					++i;
				}
			}
			if (g_auth.unlocks & UNLOCK_USED100)
			{
				long long i = RN_USED100;
				for (const auto& root_name : RootNameMgr::used100)
				{
					addOption(ret, i, root_name, LOC("RN_100"));
					++i;
				}
			}
			if (g_auth.unlocks & UNLOCK_PRE100)
			{
				long long i = RN_PRE100;
				for (const auto& root_name : RootNameMgr::pre100)
				{
					addOption(ret, i, root_name, LOC("RN_EA"));
					++i;
				}
			}
			if (g_auth.license_permissions >= LICPERM_ULTIMATE)
			{
				long long i = RN_ALL_TOXIC;
				for (const auto& root_name : RootNameMgr::all_toxic)
				{
					addOption(ret, i, root_name, LOC("RN_U"));
					++i;
				}
				if (g_auth.unlocks & UNLOCK_EARLY_TOXIC)
				{
					i = RN_EARLY_TOXIC;
					for (const auto& root_name : RootNameMgr::early_toxic)
					{
						addOption(ret, i, root_name, LOC("RN_EU"));
						++i;
					}
				}
			}
		}
		return ret;
	}

	CommandRootName::CommandRootName(CommandList* parent)
		: CommandListSelect(parent, LOC("RN"), {}, NOLABEL, getOptions(), RootNameMgr::getDefaultRootNameIdForState(), CMDFLAGS_LIST_SELECT, CMDFLAG_FEATURELIST_FINISHLIST)
	{
		instance = this;
	}

	void CommandRootName::update()
	{
		setOptions(getOptions());
		default_value = RootNameMgr::getDefaultRootNameIdForState();
	}

	void CommandRootName::onChange(Click& click, long long prev_value)
	{
		if (g_gui.canUpdateThemeSetting())
		{
			if (value == RN_NULL)
			{
				g_renderer.show_root_name = false;
				g_menu_grid.updateAddress();
			}
			else
			{
				g_renderer.show_root_name = true;
				RootNameMgr::current = value;
				g_gui.updateRootName();
			}
		}
	}

	std::string CommandRootName::getState() const
	{
		auto state = CommandListSelect::getState();
		StringUtils::replace_single(state, STAND_RN_VERSION, "{}");
		return state;
	}

	std::string CommandRootName::getDefaultState() const
	{
		auto state = CommandListSelect::getDefaultState();
		StringUtils::replace_single(state, STAND_RN_VERSION, "{}");
		return state;
	}

	void CommandRootName::setState(Click& click, const std::string& state)
	{
		std::string state_(state);
		StringUtils::replace_single(state_, "{}", STAND_RN_VERSION);
		return CommandListSelect::setState(click, state_);
	}
}
