#include "Command.hpp"

#include <soup/joaat.hpp>

#include "CommandAction.hpp"
#include "CommandInput.hpp"
#include "CommandColour.hpp"
#include "CommandDivider.hpp"
#include "CommandLink.hpp"
#include "CommandListConcealer.hpp"
#include "CommandListSelect.hpp"
#include "CommandLua.hpp"
#include "CommandPlayerListPlayersShortcut.hpp"
#include "CommandReadonlyLink.hpp"
#include "CommandReadonlyName.hpp"
#include "CommandReadonlyValue.hpp"
#include "CommandSlider.hpp"
#include "CommandSliderFloat.hpp"
#include "CommandTextsliderStateful.hpp"
#include "CommandToggle.hpp"
#include "ContextMenu.hpp"
#include "Exceptional.hpp"
#include "get_current_time_millis.hpp"
#include "Gui.hpp"
#include "MenuGrid.hpp"
#include "Renderer.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	Command::~Command() noexcept
	{
	}

	bool Command::isAttached() const noexcept
	{
		return parent != nullptr;
	}

	std::unique_ptr<Command> Command::detach()
	{
		std::unique_ptr<Command> ret{};
		EXCEPTIONAL_LOCK_READ(g_gui.root_mtx)
		for (auto i = parent->children.begin(); i != parent->children.end(); ++i)
		{
			if (i->get() == this)
			{
				EXCEPTIONAL_LOCK_WRITE(g_gui.root_mtx)
				preDetach();
				ret = std::move(*i);
				parent->children.erase(i);
				parent->processChildrenUpdate();
				EXCEPTIONAL_UNLOCK_WRITE(g_gui.root_mtx)
				break;
			}
		}
		EXCEPTIONAL_UNLOCK_READ(g_gui.root_mtx)
		ret->parent = nullptr;
		return std::move(ret);
	}

	void Command::replace(std::unique_ptr<Command>&& cmd)
	{
		EXCEPTIONAL_LOCK_READ(g_gui.root_mtx)
		for (auto& i : parent->children)
		{
			if (i.get() == this)
			{
				EXCEPTIONAL_LOCK_WRITE(g_gui.root_mtx)
				preDelete();
				i = std::move(cmd);
				parent->processChildrenUpdate();
				EXCEPTIONAL_UNLOCK_WRITE(g_gui.root_mtx)
				break;
			}
		}
		EXCEPTIONAL_UNLOCK_READ(g_gui.root_mtx)
	}

	void Command::setParent(CommandList* parent)
	{
		this->parent = parent;
		if (isT<CommandList>())
		{
			as<CommandList>()->parent_for_back.reset();
		}
	}

	template <>
	bool Command::isT<CommandPlayerClassifier>() const noexcept
	{
		return type == COMMAND_LIST_CUSTOM_SPECIAL_MEANING && parent->isT<CommandListPlayer>();
	}

	bool Command::handlesLeftRightInput() const noexcept
	{
		return type == COMMAND_LIST_SELECT
			|| isSlider()
			|| isTextslider()
			;
	}

	bool Command::isTextslider() const noexcept
	{
		return type == COMMAND_TEXTSLIDER
			|| type == COMMAND_TEXTSLIDER_STATEFUL
			;
	}

	CommandPhysical* Command::getPhysical() noexcept
	{
		if (isPhysical())
		{
			return as<CommandPhysical>();
		}
		if (isLink())
		{
			return as<CommandLink>()->getTarget();
		}
		return nullptr;
	}

	const CommandPhysical* Command::getPhysical() const noexcept
	{
		if (isPhysical())
		{
			return as<CommandPhysical>();
		}
		if (isLink())
		{
			return as<CommandLink>()->getTarget();
		}
		return nullptr;
	}

	bool Command::canBeResolved() const noexcept
	{
		return type != COMMAND_DIVIDER;
	}

	bool Command::hasParent(CommandList* target) const
	{
		for (auto list = parent; list != nullptr; list = list->parent)
		{
			if (list == target)
			{
				return true;
			}
		}
		return false;
	}

	CommandList* Command::resolveParent(const CommandType type)
	{
		CommandList* list = isT<CommandList>() ? as<CommandList>() : parent;
		while (list->type != type)
		{
			list = list->parent;
			if (list == nullptr)
			{
				break;
			}
		}
		return list;
	}

	const CommandList* Command::resolveParent(const CommandType type) const
	{
		const CommandList* list = isT<CommandList>() ? as<CommandList>() : parent;
		while (list->type != type)
		{
			list = list->parent;
			if (list == nullptr)
			{
				break;
			}
		}
		return list;
	}

	std::string Command::getPathConfig() const
	{
		std::string path(getNameForConfig());
		CommandList* list = parent;
		while (list->parent != nullptr)
		{
			path.insert(0, 1, '>');
			path.insert(0, list->getNameForConfig());
			list = list->parent;
		}
		return path;
	}

	std::string Command::getPathConfig(CommandList* target) const
	{
		std::string path(getNameForConfig());
		CommandList* list = parent;
		while (list != target)
		{
			path.insert(0, 1, '>');
			path.insert(0, list->getNameForConfig());
			list = list->parent;
		}
		return path;
	}

	std::string Command::getPathEn() const
	{
		std::string path(getMenuName().getEnglishUtf8());
		CommandList* list = parent;
		while (list->parent != nullptr)
		{
			path.insert(0, 1, '>');
			path.insert(0, list->menu_name.getEnglishUtf8());
			list = list->parent;
		}
		return path;
	}

	std::string Command::getPathEn(CommandList* target) const
	{
		std::string path(getMenuName().getEnglishUtf8());
		CommandList* list = parent;
		while (list != target)
		{
			path.insert(0, 1, '>');
			path.insert(0, list->menu_name.getEnglishUtf8());
			list = list->parent;
		}
		return path;
	}

	std::string Command::getPathEnFromParent(const CommandType type) const
	{
		std::string path(getMenuName().getEnglishUtf8());
		CommandList* list = parent;
		while (list->type != type)
		{
			path.insert(0, 1, '>');
			path.insert(0, list->menu_name.getEnglishUtf8());
			list = list->parent;
		}
		return path;
	}

	std::string Command::getPathWeb() const
	{
		std::string path(getMenuName().getWebString());
		CommandList* list = parent;
		while (list->parent != nullptr)
		{
			path.insert(0, 1, '>');
			path.insert(0, list->menu_name.getWebString());
			list = list->parent;
		}
		return path;
	}

	CommandList* Command::getParentWithContextMenuPassthrough() const
	{
		if (parent != nullptr)
		{
			return parent;
		}
		if (this == ContextMenu::root.get())
		{
			return g_gui.m_active_list[g_gui.root_cursor];
		}
		return nullptr;
	}
	
	CommandList* Command::getParentWithContextMenuPassthrough(const CommandList* tab) const
	{
		if (parent != nullptr)
		{
			if (isT<CommandList>())
			{
				auto* parent = as<CommandList>()->getParentForTab(tab);
				if (parent != nullptr)
				{
					return parent;
				}
			}
			else
			{
				return this->parent;
			}
		}
		if (this == ContextMenu::root.get())
		{
			return g_gui.m_active_list[g_gui.root_cursor];
		}
		return nullptr;
	}

	CommandList* Command::getTabInner()
	{
		CommandList* list = (isT<CommandList>() ? as<CommandList>() : getParentWithContextMenuPassthrough());
		while (!list->isTab())
		{
			list = list->getParentWithContextMenuPassthrough();
		}
		return list;
	}

	const CommandList* Command::getTabInner() const
	{
		const CommandList* list = (isT<CommandList>() ? as<CommandList>() : getParentWithContextMenuPassthrough());
		while (list != nullptr
			&& !list->isTab()
			)
		{
			list = list->getParentWithContextMenuPassthrough();
		}
		return list;
	}

	CommandList* Command::getTabInner(const CommandList* tab)
	{
		CommandList* list = (isT<CommandList>() ? as<CommandList>() : getParentWithContextMenuPassthrough(tab));
		while (!list->isTab())
		{
			list = list->getParentWithContextMenuPassthrough(tab);
		}
		return list;
	}

	CommandList* Command::getTab()
	{
		if (g_renderer.areTabsEnabled())
		{
			return getTabInner();
		}
		return g_gui.root_list.get();
	}

	const CommandList* Command::getTab() const
	{
		if (g_renderer.areTabsEnabled())
		{
			return getTabInner();
		}
		return g_gui.root_list.get();
	}

	CommandList* Command::getTab(const CommandList* tab)
	{
		if (g_renderer.areTabsEnabled())
		{
			return getTabInner(tab);
		}
		return g_gui.root_list.get();
	}

	uint8_t Command::getTabIndex() const
	{
		if (g_renderer.areTabsEnabled())
		{
			auto* const tab = getTabInner();
			for (uint8_t i = 0; i < g_gui.root_list->children.size(); i++)
			{
				if (g_gui.root_list->children[i].get() == tab)
				{
					return i;
				}
			}
		}
		return 0;
	}

	std::string Command::getLocalisedAddress() const
	{
		return getLocalisedAddress(StringUtils::utf16_to_utf8(g_renderer.address_separator));
	}

	std::string Command::getLocalisedAddress(const std::string& separator) const
	{
		if (!isAttached())
		{
			return {};
		}
		std::string path(getMenuName().getLocalisedUtf8());
		CommandList* list = parent;
		while (list->parent != nullptr)
		{
			path.insert(0, separator);
			path.insert(0, list->menu_name.getLocalisedUtf8());
			list = list->parent;
		}
		return path;
	}

	std::wstring Command::getLocalisedAddressW(const std::wstring& separator) const
	{
		if (!isAttached())
		{
			return {};
		}
		std::wstring path(getMenuName().getLocalisedUtf16());
		CommandList* list = parent;
		while (list->parent != nullptr)
		{
			path.insert(0, separator);
			path.insert(0, list->menu_name.getLocalisedUtf16());
			list = list->parent;
		}
		return path;
	}

	std::string Command::getEnglishAddress(const std::string& separator) const
	{
		if (!isAttached())
		{
			return {};
		}
		std::string path(getMenuName().getEnglishUtf8());
		CommandList* list = parent;
		while (list->parent != nullptr)
		{
			path.insert(0, separator);
			path.insert(0, list->menu_name.getEnglishUtf8());
			list = list->parent;
		}
		return path;
	}

	std::wstring Command::getEnglishAddressW(const std::wstring& separator) const
	{
		if (!isAttached())
		{
			return {};
		}
		std::wstring path(getMenuName().getEnglishUtf16());
		CommandList* list = parent;
		while (list->parent != nullptr)
		{
			path.insert(0, separator);
			path.insert(0, list->menu_name.getEnglishUtf16());
			list = list->parent;
		}
		return path;
	}

	void Command::focus(ThreadContext thread_context, bool unintrusive)
	{
		if ((!isPhysical() || !((CommandPhysical*)this)->isFocused())
			&& (parent->parent != nullptr || !g_renderer.areTabsEnabled())
			)
		{
			if (!parent->isCurrentUiList())
			{
				parent->open(thread_context, unintrusive);
			}
			focusInParent(thread_context);
		}
	}

	void Command::focusInParent(ThreadContext thread_context)
	{
		auto prev_focus = g_gui.getCurrentMenuFocus()->getWeakRef();
		parent->m_cursor = 0;
		for (const auto& entry : parent->children)
		{
			if (entry.get() == this)
			{
				break;
			}
			parent->m_cursor++;
		}
		if (parent->isCurrentUiList())
		{
			parent->fixCursorAndOffset();
			g_gui.updateFocus(thread_context, TELEPORT, prev_focus);
		}
	}

	void Command::openHotkeysList(ThreadContext thread_context)
	{
		focus(thread_context);
		ContextMenu::openIntoHotkeysList(thread_context);
	}

	cursor_t Command::getCursorPositionInParent() const
	{
		cursor_t i = 0;
		for (const auto& command : this->parent->children)
		{
			if (command.get() == this)
			{
				return i;
			}
			i++;
		}
		return 0;
	}

	void Command::beGone()
	{
		EXCEPTIONAL_LOCK_WRITE(g_gui.root_mtx)
		beGoneRootWriteLocked();
		EXCEPTIONAL_UNLOCK_WRITE(g_gui.root_mtx)
	}

	void Command::beGoneRootWriteLocked()
	{
		CommandList* const parent = this->parent;
		for (auto i = parent->children.begin(); i != parent->children.end(); i++)
		{
			if (i->get() == this)
			{
				preDelete();
				parent->children.erase(i);
				parent->processChildrenUpdate();
				break;
			}
		}
	}

	void Command::preDelete()
	{
		preDetach();
	}

	void Command::preDetach()
	{
		if (parent->m_cursor > getCursorPositionInParent())
		{
			--parent->m_cursor;
		}
	}

	const Label& Command::getMenuName() const
	{
		if (auto* physical = getPhysical())
		{
			return physical->menu_name;
		}
		return Label::sNoLabel;
	}

	std::string Command::getNameForConfig() const
	{
		if (auto* physical = getPhysical())
		{
			return physical->getNameForConfig();
		}
		return {};
	}

	std::wstring Command::getMenuNameForAddress() const
	{
		std::wstring name = getMenuName().getLocalisedUtf16();
		StringUtils::replace_all(name, L"\n", L" ");
		return name;
	}

	uint32_t Command::getHash() const
	{
		return rage::atStringHash(getPathWeb());
	}

	bool Command::shouldShowUntrimmedName() const
	{
		switch (type)
		{
		case COMMAND_TOGGLE:
			return !as<CommandToggle>()->correlation.isActive();

		case COMMAND_LIST_SELECT:
			return as<CommandListSelect>()->getCurrentValueHelpText().empty();
		}
		return true;
	}

	void Command::populateCorner(std::vector<std::wstring>& corner) const
	{
		if (isPhysical())
		{
			as<CommandPhysical>()->populateCorner(corner);
		}
		else if (isLink())
		{
			auto* physical = as<CommandLink>()->getTarget();
			if (physical != nullptr)
			{
				physical->populateCorner(corner);
				if (as<CommandLink>()->show_address_in_corner)
				{
					auto addr = physical->getLocalisedAddressW(g_renderer.address_separator);
					if (!addr.empty())
					{
						corner.emplace_back(std::move(addr));
					}
				}
			}
		}
	}

	std::wstring Command::getGenericIrreversableWarningLabel()
	{
		return Label::combineWithSpace(LOC("GENWARN"), LOC("IRREV")).getLocalisedUtf16();
	}

	void Command::showWarning(Click& click, std::wstring&& message, warningflags_t flags, std::function<void(ThreadContext)>&& proceed_callback, std::function<void(ThreadContext)>&& cancel_callback)
	{
		auto warning_hash = (uint64_t)(uint32_t)soup::joaat::hashRange(reinterpret_cast<const char*>(&message.at(0)), message.size() * sizeof(wchar_t));
		if (flags & WARNING_COMMAND_BOUND)
		{
			warning_hash |= (uint64_t(getHash()) << 32);
		}
		if (click.shouldAutomaticallyProceedOnWarning(flags & WARNING_SKIPPABLE)
			|| (g_gui.skip_accepted_warnings && g_gui.accepted_warnings.find(warning_hash) != g_gui.accepted_warnings.end()))
		{
			proceed_callback(click.thread_context);
		}
		else
		{
			click.setNoResponse();
			focus(click.thread_context);
			click.sound_feedback = SOUND_ERROR;
			time_t can_proceed_after = 0;
			if (g_gui.force_warning_read && !(flags & WARNING_SKIPPABLE))
			{
				can_proceed_after = get_current_time_millis() + 1'000;
			}
			g_gui.getCurrentUiFocusPhysical()->onBlur(click.thread_context, FORWARD);
			g_gui.warnings.emplace(g_gui.root_cursor, WarningData{
				warning_hash,
				std::move(message),
				std::move(can_proceed_after),
				std::move(proceed_callback),
				std::move(cancel_callback)
			});
			g_menu_grid.update();
		}
	}

	void Command::showWarningA(Click& click, const std::string& message, warningflags_t flags, std::function<void(ThreadContext)>&& proceed_callback, std::function<void(ThreadContext)>&& cancel_callback)
	{
		showWarning(click, StringUtils::utf8_to_utf16(message), flags, std::move(proceed_callback), std::move(cancel_callback));
	}

	void Command::warnSelfHarm(Click& click, std::function<void(ThreadContext)>&& proceed_callback)
	{
		if (click.issuedByUser())
		{
			showWarning(click, LANG_GET_W("SLFHRM"), 0, std::move(proceed_callback));
		}
		else
		{
			click.setResponse(LOC("SLFHRM_O"));
		}
	}

	CommandLuaData* Command::getLuaData() noexcept
	{
#ifdef STAND_DEBUG
		SOUP_ASSERT(hasLuaData());
#endif
		switch (type)
		{
		case COMMAND_ACTION:
			return &((CommandLua<CommandAction>*)this)->lua_data;
		case COMMAND_INPUT:
			return &((CommandLua<CommandInput>*)this)->lua_data;
		case COMMAND_DIVIDER:
			return &((CommandLua<CommandDivider>*)this)->lua_data;
		case COMMAND_LIST:
			return &((CommandLua<CommandPlayerListPlayersShortcut>*)this)->lua_data;
		case COMMAND_LIST_COLOUR:
			return &((CommandLua<CommandColour>*)this)->lua_data;
		case COMMAND_LIST_CONCEALER:
			return &((CommandLua<CommandListConcealer>*)this)->lua_data;
		case COMMAND_TOGGLE:
			return &((CommandLua<CommandToggle>*)this)->lua_data;
		case COMMAND_SLIDER:
			return &((CommandLua<CommandSlider>*)this)->lua_data;
		case COMMAND_SLIDER_FLOAT:
			return &((CommandLua<CommandSliderFloat>*)this)->lua_data;
		case COMMAND_TEXTSLIDER:
			return &((CommandLua<CommandTextslider>*)this)->lua_data;
		case COMMAND_TEXTSLIDER_STATEFUL:
			return &((CommandLua<CommandTextsliderStateful>*)this)->lua_data;
		case COMMAND_LINK:
			return &((CommandLua<CommandLink>*)this)->lua_data;
		case COMMAND_LIST_ACTION:
			return &((CommandLua<CommandListAction>*)this)->lua_data;
		case COMMAND_LIST_SELECT:
			return &((CommandLua<CommandListSelect>*)this)->lua_data;
		case COMMAND_READONLY_NAME:
			return &((CommandLua<CommandReadonlyName>*)this)->lua_data;
		case COMMAND_READONLY_VALUE:
			return &((CommandLua<CommandReadonlyValue>*)this)->lua_data;
		case COMMAND_READONLY_LINK:
			return &((CommandLua<CommandReadonlyLink>*)this)->lua_data;
		}
		SOUP_ASSERT_UNREACHABLE;
		return nullptr;
	}

	const CommandLuaData* Command::getLuaData() const noexcept
	{
		return const_cast<Command*>(this)->getLuaData();
	}
}
