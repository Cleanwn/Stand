#include "CommandList.hpp"

#include <algorithm>

#include <fmt/xchar.h>

#include "alphanum.hpp"
#include "CommandInput.hpp"
#include "CommandListConcealer.hpp"
#include "CommandListSelect.hpp"
#include "CommandReadonlyValueCopy.hpp"
#include "CommandSlider.hpp"
#include "CommandTextsliderStateful.hpp"
#include "CommandToggle.hpp"
#include "ContextMenu.hpp"
#include "Exceptional.hpp"
#include "ExecCtx.hpp"
#include "GridItemList.hpp"
#include "Gui.hpp"
#include "MenuGrid.hpp"
#include "pointers.hpp"
#include "RelayCon.hpp"
#include "Renderer.hpp"
#include "StringUtils.hpp"
#include "Util.hpp"

namespace Stand
{
	CommandList::CommandList(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, const commandflags_t flags, const CommandType type)
		: CommandPhysical(type, parent, std::move(menu_name), std::move(command_names), std::move(help_text), flags)
	{
	}

	bool CommandList::isRoot() const noexcept
	{
		return parent == nullptr;
	}

	void CommandList::preDelete()
	{
		CommandPhysical::preDelete();

		dispatchPreDeleteForAllChildren();
	}

	void CommandList::preDetach()
	{
		CommandPhysical::preDetach();

		evacuateCursor();
	}

	void CommandList::dispatchPreDeleteForAllChildren() const
	{
		for (const auto& child : children)
		{
			child->preDelete();
		}
	}

	void CommandList::resetChildrenWithPreDelete()
	{
		dispatchPreDeleteForAllChildren();
		resetChildren();
	}

	void CommandList::resetChildren() noexcept
	{
		if (!children.empty())
		{
			{
				auto focus = g_gui.getCurrentUiFocus();
				if (focus != nullptr && focus->parent == this)
				{
					if (auto phys = focus->getPhysical())
					{
						phys->onBlur(TC_OTHER, TELEPORT);
					}
				}
			}
			EXCEPTIONAL_LOCK_WRITE(g_gui.root_mtx)
			children.clear();
			EXCEPTIONAL_UNLOCK_WRITE(g_gui.root_mtx)
			dividers = 0;
		}
	}

	void CommandList::processChildrenUpdate()
	{
		__try
		{
			if (isCurrentUiList())
			{
#if false // this works poorly with mouse mode
				if (m_cursor < (cursor_t)countVisibleChildren())
				{
					processChildrenFocusUpdate(TC_OTHER, TELEPORT);
				}
				else
				{
					fixCursor();
				}
				fixOffset();
#else
				fixCursorAndOffset(g_gui.mouse_mode == MouseMode::NAVIGATE);
#endif
				g_menu_grid.update();
			}
			if (isActiveOnWeb())
			{
				updateWebState();
			}
		}
		__EXCEPTIONAL()
		{
		}
	}

	void CommandList::processChildrenVisualUpdate()
	{
		if (isCurrentUiList())
		{
			g_menu_grid.update();
		}
		if (isActiveOnWeb())
		{
			updateWebState();
		}
	}

	void CommandList::processChildrenUpdateWithPossibleCursorDisplacement(Command* prev_focus, ThreadContext tc)
	{
		if (prev_focus != nullptr)
		{
			prev_focus->focusInParent(tc);
		}
		else if (isCurrentUiList())
		{
			fixCursorAndOffset();
		}
	}

	void CommandList::emplaceVisible(std::unique_ptr<Command>&& cmd)
	{
		if (isT<CommandListConcealer>())
		{
			as<CommandListConcealer>()->emplaceVisible(std::move(cmd));
		}
		else
		{
#ifdef STAND_DEBUG
			SOUP_ASSERT(g_gui.root_mtx.isWriteLockedByThisThread());
#endif
			children.emplace_back(std::move(cmd));
		}
	}

	void CommandList::removeChild(std::vector<std::unique_ptr<Command>>::iterator it)
	{
		(*it)->preDelete();
#ifdef STAND_DEBUG
		SOUP_ASSERT(g_gui.root_mtx.isWriteLockedByThisThread());
#endif
		children.erase(it);
		processChildrenUpdate();
	}

	bool CommandList::canDispatchOnTickInViewportForChildren() const noexcept
	{
		return !(flags & CMDFLAG_CUSTOM_ON_TICK_IN_VIEWPORT);
	}

	bool CommandList::canDispatchOnTickInWebViewportForChildren() const noexcept
	{
		return children.size() < max_web_commands;
	}

	void CommandList::onClick(Click& click)
	{
		if (!isAttached())
		{
			onClickImplUnavailable(click);
			return;
		}
		if (click.isWeb())
		{
			if (g_gui.web_focus != this)
			{
				Exceptional::createManagedExceptionalThread(__FUNCTION__, [this]
				{
					EXCEPTIONAL_LOCK(g_relay.send_mtx)
					auto* const prev_focus = g_gui.web_focus;
					g_gui.web_focus = this;
					g_relay.sendLine(std::move(std::string("l ").append(menu_name.getWebString())));
					updateWebStateImpl();
					if (prev_focus != nullptr)
					{
						prev_focus->onActiveListUpdate();
					}
					onActiveListUpdate();
					EXCEPTIONAL_UNLOCK(g_relay.send_mtx)
				});
			}
		}
		else
		{
			this->open(click.thread_context, false, false, click.type != CLICK_MENU);
		}
	}

	void CommandList::onClickImplUnavailable(Click& click)
	{
		click.setUnavailable();
		if (click.type == CLICK_WEB)
		{
			g_gui.web_focus->updateWebStateNoCheck();
		}
	}

	void CommandList::open(ThreadContext thread_context, bool unintrusive, bool without_grid_update, bool can_update_root_cursor)
	{
		g_gui.lerpStop();
		if (getRoot() == ContextMenu::root.get())
		{
			g_gui.inputStop();
			if (resetCursorIfApplicable())
			{
				fixCursorAndOffset();
			}
			ContextMenu::view = this;
			g_gui.updateFocusedList(thread_context, FORWARD, this);
		}
		else
		{
			if (!unintrusive)
			{
				g_gui.openForcefully(thread_context);
			}
			g_gui.inputStop();
			if (resetCursorIfApplicable())
			{
				fixCursorAndOffset();
			}
			auto prev_focus = g_gui.getCurrentMenuFocus()->getWeakRef();
			const auto my_tab_idx = getTabIndex();
			if (g_gui.root_cursor != my_tab_idx
				&& (can_update_root_cursor || isTab())
				)
			{
				g_gui.root_cursor = my_tab_idx;
				g_gui.updateFocus(thread_context, TELEPORT, prev_focus);
				prev_focus = g_gui.getCurrentMenuFocus()->getWeakRef();
			}
			g_gui.m_active_list[g_gui.root_cursor] = this;
			onActiveListUpdate();
			g_gui.updateFocus(thread_context, FORWARD, prev_focus);
		}
		if (!without_grid_update)
		{
			g_menu_grid.update();
		}
	}

	void CommandList::redirectOpen(Click& click, const CommandPhysical* cmd)
	{
		redirectOpen(click.thread_context, cmd);
	}

	void CommandList::redirectOpen(ThreadContext tc, const CommandPhysical* cmd)
	{
		parent_for_back = cmd->parent->getWeakRef();
		open(tc, false, false, false);
	}

	void CommandList::redirectOpen(const CommandPhysical* cmd)
	{
		redirectOpen(ExecCtx::get().tc, cmd);
	}

	CommandList* CommandList::getParentForBack(CommandList* tab)
	{
		if (auto parent_for_back = this->getParentForBackIfApplicable(tab))
		{
			this->parent_for_back.reset();
			return parent_for_back;
		}

		if (g_renderer.areTabsEnabled()
			&& !isTab()
			&& parent->getTab(tab) != tab // could be possible by using "In Player History" and Player History's Noted, Tracked, etc.
			)
		{
			return tab;
		}

		return parent;
	}

	CommandList* CommandList::getParentForBackIfApplicable(const CommandList* const tab) const
	{
		if (parent_for_back)
		{
			if (auto ptr = parent_for_back.getPointer())
			{
				if (ptr->getTab() == tab)
				{
					return ptr->as<CommandList>();
				}
			}
		}
		return nullptr;
	}

	CommandList* CommandList::getParentForTab(const CommandList* const tab) const
	{
		if (parent_for_back)
		{
			if (auto ptr = parent_for_back.getPointer())
			{
				if (ptr->getTab() == tab)
				{
					return ptr->as<CommandList>();
				}
			}
		}
		return parent;
	}

	void CommandList::setIndicatorType(ListIndicatorType t)
	{
		if (indicator_type != t)
		{
			indicator_type = t;
			processVisualUpdate(true);
		}
	}

	void CommandList::fixCursorAndOffset(bool no_padding)
	{
		if (fixCursor())
		{
			fixOffset(no_padding);
		}
	}

	bool CommandList::fixCursor()
	{
		const cursor_t max_cursor = (cursor_t)countVisibleChildren() - 1;

		if (m_cursor > max_cursor)
		{
			if (max_cursor < 0)
			{
				return false;
			}
			m_cursor = max_cursor;
			if (isCurrentUiList())
			{
				processFocusUpdate(TC_OTHER, TELEPORT);
			}
		}
		else if (m_cursor < 0)
		{
			m_cursor = 0;
			m_offset = 0;
			if (isCurrentUiList())
			{
				processFocusUpdate(TC_OTHER, TELEPORT);
			}
			return false;
		}

		return true;
	}

	void CommandList::fixOffset(bool no_padding)
	{
		const auto num_children = (cursor_t)countVisibleChildren();
		const cursor_t max_cursor = num_children - 1;
		const cursor_t cursor_padding = (no_padding ? 0 : g_gui.getCursorPadding());
		const cursor_t on_screen_limit = g_gui.getCommandsOnScreenLimit();

		if (m_offset > m_cursor - cursor_padding)
		{
			m_offset = m_cursor - cursor_padding;
		}
		if (m_offset < m_cursor - on_screen_limit + 1 + cursor_padding || (m_cursor > max_cursor - cursor_padding && m_offset == max_cursor - on_screen_limit))
		{
			m_offset = m_cursor - on_screen_limit + 1 + cursor_padding;
		}
		if (m_offset < 0 || num_children <= on_screen_limit)
		{
			m_offset = 0;
		}
		const cursor_t max_offset = num_children > on_screen_limit ? num_children - on_screen_limit : 0;
		if (m_offset >= max_offset)
		{
			m_offset = max_offset;
		}
	}

	void CommandList::processFocusUpdate(ThreadContext thread_context, Direction momentum, bool user_action)
	{
		// We don't wanna do onActiveListUpdate here because this also gets called when navigating up/down in the list.

		if (auto new_focus = getFocus())
		{
			if (auto physical = new_focus->getPhysical())
			{
				physical->onFocus(thread_context, momentum);
			}

			/*Util::toast(fmt::format("{}, {}, {}, {}, {}, {}",
				user_action,
				g_gui.mouse_mode == MouseMode::NAVIGATE,
				g_gui.mouse_mode_set_focus,
				g_gui.last_input_type == INPUTTYPE_INDIFFERENT,
				new_focus->getPhysical()->isSelectable(),
				GetForegroundWindow() == pointers::hwnd
			));*/

			if (user_action
				&& g_gui.mouse_mode == MouseMode::NAVIGATE
				&& g_gui.mouse_mode_set_focus
				&& g_gui.last_input_type == INPUTTYPE_INDIFFERENT
				&& new_focus->getPhysical()->isSelectable()
				&& GetForegroundWindow() == pointers::hwnd
				)
			{
				setCursorPos(new_focus);
			}
		}
	}

	void CommandList::onActiveListUpdate()
	{
		if (parent != nullptr)
		{
			parent->onActiveListUpdate();
		}
	}

	void CommandList::resetCursor() noexcept
	{
		m_cursor = 0;
		m_offset = 0;
	}

	bool CommandList::resetCursorIfApplicable() noexcept
	{
		if (type != COMMAND_LIST_ACTION && (
			g_gui.reset_cursor_on_back
			|| (g_gui.keep_cursor_reduced_for_huge_lists && children.size() > 100 && m_cursor > g_gui.command_rows)
			))
		{
			resetCursor();
			return false;
		}
		return true;
	}

	void CommandList::onBack(ThreadContext thread_context)
	{
	}

	bool CommandList::isCurrentUiList() const
	{
		return g_gui.getCurrentUiList() == this;
	}

	bool CommandList::isCurrentMenuList() const
	{
		return g_gui.getCurrentMenuList() == this;
	}

	bool CommandList::isCurrentWebList() const
	{
		return g_gui.web_focus == this;
	}

	bool CommandList::isCurrentUiOrWebList() const
	{
		return g_gui.web_focus == this || isCurrentUiList();
	}

	bool CommandList::isCurrentMenuOrWebList() const
	{
		return g_gui.web_focus == this || isCurrentMenuList();
	}

	bool CommandList::isCurrentMenuListInTab() const
	{
		return g_gui.getCurrentMenuList(getTabIndex()) == this;
	}

	bool CommandList::isThisOrSublist(const CommandList* list) const
	{
		const CommandList* const tab = getTab();
		do
		{
			if (list == this)
			{
				return true;
			}
			list = list->getParentForTab(tab);
		} while (list != nullptr);
		return false;
	}

	bool CommandList::isThisOrSublistInTab(const CommandList* list) const
	{
		const CommandList* const tab = list->getTab();
		do
		{
			if (list == this)
			{
				return true;
			}
			list = list->getParentForTab(tab);
		} while (list != nullptr);
		return false;
	}

	bool CommandList::isThisOrSublistCurrentUiList() const
	{
		return isThisOrSublist(g_gui.getCurrentUiList());
	}

	bool CommandList::isThisOrSublistCurrentMenuList() const
	{
		return isThisOrSublist(g_gui.getCurrentMenuList());
	}

	bool CommandList::isThisOrSublistCurrentWebList() const
	{
		if (g_gui.web_focus != nullptr)
		{
			return isThisOrSublist(g_gui.web_focus);
		}
		return false;
	}

	bool CommandList::isThisOrSublistCurrentUiOrWebList() const
	{
		return isThisOrSublistCurrentUiList()
			|| isThisOrSublistCurrentWebList()
			;
	}

	bool CommandList::isThisOrSublistCurrentMenuOrWebList() const
	{
		return isThisOrSublistCurrentMenuList()
			|| isThisOrSublistCurrentWebList()
			;
	}

	bool CommandList::isThisOrSublistCurrentUiListInTab() const
	{
		if (isThisOrSublistInTab(g_gui.getCurrentUiList()))
		{
			return true;
		}
		if (g_gui.web_focus != nullptr)
		{
			return isThisOrSublistInTab(g_gui.web_focus);
		}
		return false;
	}

	bool CommandList::isThisOrSublistActiveInMyTabMenu() const
	{
		if (isThisOrSublist(g_gui.getCurrentMenuList(getTabIndex())))
		{
			return true;
		}
		if (g_gui.web_focus != nullptr)
		{
			return isThisOrSublist(g_gui.web_focus);
		}
		return false;
	}

	bool CommandList::isThisOrSublistCurrentMenuListInTab() const
	{
		if (isThisOrSublistInTab(g_gui.getCurrentMenuList(getTabIndex())))
		{
			return true;
		}
		if (g_gui.web_focus != nullptr)
		{
			return isThisOrSublistInTab(g_gui.web_focus);
		}
		return false;
	}

	void CommandList::goBackIfActive(ThreadContext thread_context)
	{
		if (isCurrentUiList())
		{
			g_gui.inputBack(thread_context, false);
		}
		else
		{
			onBack(thread_context);
		}
		if (g_gui.web_focus == this)
		{
			Exceptional::createManagedExceptionalThread(__FUNCTION__, []
			{
				EXCEPTIONAL_LOCK(g_relay.send_mtx)
				g_gui.webGoBack();
				EXCEPTIONAL_UNLOCK(g_relay.send_mtx)
			});
		}
	}

	bool CommandList::isInViewport(const CommandPhysical* const physical) const
	{
		bool ret = false;
		EXCEPTIONAL_LOCK_READ(g_gui.root_mtx)
		ret = isInViewportRootReadlocked(physical);
		EXCEPTIONAL_UNLOCK_READ(g_gui.root_mtx)
		return ret;
	}

	bool CommandList::isInViewportRootReadlocked(const CommandPhysical* const physical) const
	{
		cursor_t draw_cursor = 0;
		auto i = children.begin() + m_offset;
		while (i != children.end())
		{
			if ((*i)->getPhysical() == physical)
			{
				return true;
			}
			if (++draw_cursor == g_gui.getCommandsOnScreenLimit())
			{
				break;
			}
			++i;
		}
		return false;
	}

	bool CommandList::canUpdateCursor() const
	{
		if (g_gui.last_input_type != INPUTTYPE_INDIFFERENT)
		{
			return false;
		}
		for (const auto& child : children)
		{
			if (child->getPhysical()->isSelectable())
			{
				return true;
			}
		}
		return false;
	}

	void CommandList::approachCursor(cursor_t desired)
	{
		if (m_cursor == desired)
		{
			return;
		}
		g_gui.last_input_type = INPUTTYPE_MOUSE_MOVE;
		if (m_cursor > desired)
		{
			g_gui.inputUp(TC_RENDERER, true);
		}
		else
		{
			g_gui.inputDown(TC_RENDERER, true);
		}
	}

	void CommandList::evacuateCursor()
	{
		for (uint8_t tab_idx = 0; tab_idx != (uint8_t)g_gui.m_active_list.size(); ++tab_idx)
		{
			evacuateCursorTab(tab_idx);
		}
		evacuateCursorWeb();
	}

	void CommandList::evacuateCursorTab(uint8_t tab_idx)
	{
		CommandList* const tab = g_gui.root_list->children.at(tab_idx)->as<CommandList>();
		const CommandList* list = g_gui.m_active_list[tab_idx];
		int8_t back_count = 0;
		CommandPhysical* ctx_list = nullptr;
		if (ContextMenu::isOpen())
		{
			ctx_list = g_gui.getCurrentMenuList();
		}
		do
		{
			if (list == this)
			{
				CommandList* prev_active_list;
				do
				{
					if (g_gui.m_active_list[tab_idx] == ctx_list)
					{
						ContextMenu::close(TC_OTHER);
					}
					prev_active_list = g_gui.m_active_list[tab_idx];
					g_gui.m_active_list[tab_idx] = g_gui.m_active_list[tab_idx]->getParentForBack(tab);
				} while (back_count-- > 0);
				if (g_gui.m_active_list[tab_idx]->isCurrentUiList())
				{
					g_gui.m_active_list[tab_idx]->updateCursorGoingBack(prev_active_list);
				}
				break;
			}
			list = list->parent;
			back_count++;
		} while (list != nullptr);
	}

	void CommandList::evacuateCursorWeb()
	{
		if (g_gui.isWebGuiActive())
		{
			int8_t back_count = 1;
			const CommandList* list = g_gui.web_focus;
			do
			{
				if (list == this)
				{
					Exceptional::createManagedExceptionalThread(__FUNCTION__, [back_count]
					{
						EXCEPTIONAL_LOCK(g_relay.send_mtx)
						g_gui.webGoBack(back_count);
						EXCEPTIONAL_UNLOCK(g_relay.send_mtx)
					});
					break;
				}
				list = (const CommandList*)list->parent;
				back_count++;
			} while (list != nullptr);
		}
	}

	void CommandList::evacuateCursorStandalone()
	{
		if (isThisOrSublistCurrentUiListInTab())
		{
			evacuateCursorStandaloneNocheck();
		}
	}

	void CommandList::evacuateCursorStandaloneNocheck()
	{
		evacuateCursor();
		g_menu_grid.update();
	}

	void CommandList::updateCursorGoingBack(CommandList* prev_active_list)
	{
		SOUP_IF_UNLIKELY (g_renderer.areTabsEnabled() && this == g_gui.root_list.get())
		{
			Exceptional::report(soup::ObfusString("Backed into root list despite tabs being enabled"));
		}

		const auto prev_cursor = m_cursor;
		m_cursor = 0;
		bool found = false;
		forEachChildAsPhysical([&found, this, prev_active_list](CommandPhysical* command)
		{
			if (command == prev_active_list)
			{
				found = true;
				CONSUMER_BREAK;
			}
			++this->m_cursor;
			CONSUMER_CONTINUE;
		});
		if (!found)
		{
			found = false;
			forEachChildAsPhysical([&found, this, prev_active_list](CommandPhysical* command)
			{
				if (command->menu_name == prev_active_list->menu_name)
				{
					found = true;
					CONSUMER_BREAK;
				}
				++this->m_cursor;
				CONSUMER_CONTINUE;
			});
			if (!found)
			{
				m_cursor = prev_cursor;
			}
		}
		fixCursorAndOffset();
	}

	void CommandList::updateWebState()
	{
		if (isActiveOnWeb())
		{
			updateWebStateNoCheck();
		}
	}

	void CommandList::updateWebStateNoCheck()
	{
		if (web_state_update_queued)
		{
			return;
		}
		web_state_update_queued = true;
		Exceptional::createManagedExceptionalThread(__FUNCTION__, [this]
		{
			web_state_update_queued = false;
			EXCEPTIONAL_LOCK(g_relay.send_mtx)
			g_relay.sendRaw("j\n");
			updateWebStateImpl();
			EXCEPTIONAL_UNLOCK(g_relay.send_mtx)
		});
	}

	void CommandList::updateWebStateImpl() const
	{
		EXCEPTIONAL_LOCK_READ(g_gui.root_mtx)
		cursor_t cap = 0;
		for (auto& command : children)
		{
			auto* physical = command->getPhysical();
			if (physical->flags & CMDFLAG_CONCEALED)
			{
				break;
			}
			if (physical->isToggle())
			{
				g_relay.sendLine(std::move(std::string("t ").append(((const CommandToggleNoCorrelation*)command.get())->m_on ? "1" : "0").append(physical->menu_name.getWebString())));
			}
			else if (physical->isSlider())
			{
				auto slider = (const CommandSlider*)physical;
				std::string msg{ "z " };
				msg.append(slider->menu_name.getWebString()).push_back(':');
				msg.append(fmt::to_string(slider->min_value)).push_back(':');
				msg.append(fmt::to_string(slider->max_value)).push_back(':');
				msg.append(fmt::to_string(slider->step_size)).push_back(':');
				msg.append(fmt::to_string(slider->value));
				g_relay.sendLine(std::move(msg));
			}
			else if (physical->type == COMMAND_INPUT)
			{
				g_relay.sendLine(std::move(std::string("v ").append(physical->menu_name.getWebString()).append("\ns ").append(((const CommandInput*)physical)->value)));
			}
			else if (physical->type == COMMAND_DIVIDER)
			{
				g_relay.sendLine(std::move(std::string("- ").append(physical->menu_name.getWebString())));
			}
			else if (physical->type == COMMAND_READONLY_NAME)
			{
				g_relay.sendLine(std::move(std::string("r ").append(physical->menu_name.getWebString())));
			}
			else if (physical->type == COMMAND_READONLY_VALUE)
			{
				std::string msg{ "r " };
				msg.append(physical->menu_name.getWebString()).push_back(':');
				msg.append(StringUtils::utf16_to_utf8(((const CommandReadonlyValueCopy*)physical)->value));
				g_relay.sendLine(std::move(msg));
			}
			else if (physical->type == COMMAND_LIST_SELECT)
			{
				physical->as<CommandListSelect>()->sendDataToRelay();
			}
			else if (physical->type == COMMAND_TEXTSLIDER_STATEFUL)
			{
				g_relay.sendLine(std::move(std::string("d ").append(physical->menu_name.getWebString())));
				for (const auto& options : physical->as<CommandTextslider>()->options)
				{
					g_relay.sendLine(std::move(std::string("> ").append(options.label.getWebString())));
				}
				g_relay.sendLine(std::move(std::string("s ").append(physical->as<CommandTextsliderStateful>()->getWebState())));
			}
			else if (physical->isList())
			{
				g_relay.sendLine(std::move(std::string("i ").append(physical->menu_name.getWebString())));
			}
			else
			{
				g_relay.sendLine(std::move(std::string("a ").append(physical->menu_name.getWebString())));
			}
			if (physical->hasHelpTextForListing())
			{
				if (physical->help_text.isLiteral())
				{
					std::string line = std::string("h ").append(physical->help_text.literal_str);
					StringUtils::replace_all(line, "\n", (const char*)u8" • ");
					g_relay.sendLine(std::move(line));
				}
				else
				{
					g_relay.sendLine(std::move(std::string("h ").append(physical->help_text.getWebString())));
				}
			}
			if (!physical->command_names.empty())
			{
				std::string data(1, 'c');
				for (const auto& command : physical->command_names)
				{
					data.push_back(' ');
					data.append(cmdNameToUtf8(command));
				}
				g_relay.sendLine(std::move(data));
			}
			if (physical->type == COMMAND_INPUT)
			{
				g_relay.sendRaw("v\n");
			}
			if (++cap == max_web_commands)
			{
				break;
			}
		}
		EXCEPTIONAL_UNLOCK_READ(g_gui.root_mtx)
	}

	Command* CommandList::getFocus() const
	{
		if (m_cursor >= 0 && m_cursor < (cursor_t)children.size())
		{
			return children.at(m_cursor).get();
		}
		return nullptr;
	}

	CommandPhysical* CommandList::getFocusPhysical() const
	{
		auto* focus = getFocus();
		if (focus != nullptr)
		{
			return focus->getPhysical();
		}
		return nullptr;
	}

	Command* CommandList::getChild(CommandType type) const
	{
		for (const auto& command : this->children)
		{
			if (command->type == type)
			{
				return command.get();
			}
		}
		return nullptr;
	}

	CommandPhysical* CommandList::resolveChildByMenuName(const Label& target) const
	{
		for (const auto& _command : this->children)
		{
			if (!_command->isT<CommandPhysical>())
			{
				continue;
			}
			auto command = _command->as<CommandPhysical>();
			if (command->menu_name == target && command->canBeResolved())
			{
				return command;
			}
		}
		return nullptr;
	}

	CommandPhysical* CommandList::resolveChildByMenuNameHash(hash_t target) const
	{
		for (const auto& _command : this->children)
		{
			if (!_command->isT<CommandPhysical>())
			{
				continue;
			}
			auto command = _command->as<CommandPhysical>();
			if (command->menu_name == target && command->canBeResolved())
			{
				return command;
			}
		}
		return nullptr;
	}

	CommandPhysical* CommandList::resolveChildByMenuNameLiteral(const std::string& target) const
	{
		for (const auto& _command : this->children)
		{
			if (!_command->isT<CommandPhysical>())
			{
				continue;
			}
			auto command = _command->as<CommandPhysical>();
			if (command->menu_name.isLiteralString(target) && command->canBeResolved())
			{
				return command;
			}
		}
		return nullptr;
	}

	CommandPhysical* CommandList::resolveChildByMenuNameWebString(const std::string& target) const
	{
		for (const auto& _command : this->children)
		{
			auto command = _command->getPhysical();
			if (command->menu_name.getWebString() == target && command->canBeResolved())
			{
				return command;
			}
		}
		return nullptr;
	}

	CommandPhysical* CommandList::recursivelyResolveChildByMenuName(const Label& target) const
	{
		for (const auto& _command : this->children)
		{
			if (!_command->isT<CommandPhysical>())
			{
				continue;
			}
			auto command = _command->as<CommandPhysical>();
			if (command->menu_name == target && command->canBeResolved())
			{
				return command;
			}
			if (command->isT<CommandList>())
			{
				auto res = command->as<CommandList>()->recursivelyResolveChildByMenuName(target);
				if (res != nullptr)
				{
					return res;
				}
			}
		}
		return nullptr;
	}

	CommandPhysical* CommandList::recursivelyResolveChildByMenuNameHash(hash_t target) const
	{
		for (const auto& _command : this->children)
		{
			if (!_command->isT<CommandPhysical>())
			{
				continue;
			}
			auto command = _command->as<CommandPhysical>();
			if (command->menu_name == target && command->canBeResolved())
			{
				return command;
			}
			if (command->isT<CommandList>())
			{
				auto res = command->as<CommandList>()->recursivelyResolveChildByMenuNameHash(target);
				if (res != nullptr)
				{
					return res;
				}
			}
		}
		return nullptr;
	}

	CommandPhysical* CommandList::resolveCommandConfig(const std::string& target, const std::string& prefix) const
	{
		for (const auto& _command : this->children)
		{
			if (!_command->isT<CommandPhysical>())
			{
				continue;
			}
			auto command = _command->as<CommandPhysical>();
			if (!command->canBeResolved())
			{
				continue;
			}
			std::string name = std::string(prefix).append(command->getNameForConfig());
			if (target.substr(0, name.length()) != name)
			{
				continue;
			}
			if (target == name)
			{
				return command;
			}
			if (command->isT<CommandList>())
			{
				name.push_back('>');
				auto res = command->as<CommandList>()->resolveCommandConfig(target, name);
				if (res != nullptr)
				{
					return res;
				}
			}
		}
		return nullptr;
	}

	CommandPhysical* CommandList::resolveCommandEnglish(const std::string& target, const std::string& prefix) const
	{
		for (const auto& _command : this->children)
		{
			if (!_command->isT<CommandPhysical>())
			{
				continue;
			}
			auto command = _command->as<CommandPhysical>();
			if (!command->canBeResolved())
			{
				continue;
			}
			std::string name = std::string(prefix).append(command->menu_name.getEnglishUtf8());
			if (target.substr(0, name.length()) != name)
			{
				continue;
			}
			if (target == name)
			{
				return command;
			}
			if (command->isT<CommandList>())
			{
				name.push_back('>');
				auto res = command->as<CommandList>()->resolveCommandEnglish(target, name);
				if (res != nullptr)
				{
					return res;
				}
			}
		}
		return nullptr;
	}

	CommandPhysical* CommandList::resolveCommandWeb(const std::string& target, const std::string& prefix) const
	{
		for (const auto& _command : this->children)
		{
			if (!_command->isT<CommandPhysical>())
			{
				continue;
			}
			auto command = _command->as<CommandPhysical>();
			if (!command->canBeResolved())
			{
				continue;
			}
			std::string name = std::string(prefix).append(command->menu_name.getWebString());
			if (target.substr(0, name.length()) != name)
			{
				continue;
			}
			if (target == name)
			{
				return command;
			}
			if (command->isT<CommandList>())
			{
				name.push_back('>');
				auto * res = command->as<CommandList>()->resolveCommandWeb(target, name);
				if (res != nullptr)
				{
					return res;
				}
			}
		}
		return nullptr;
	}

	int8_t CommandList::checkCommandNameMatch(const CommandName& command_name_prefix, const CommandName& command_name)
	{
		if (command_name.length() >= command_name_prefix.length())
		{
			if (command_name.substr(0, command_name_prefix.length()) == command_name_prefix)
			{
				if (command_name.length() == command_name_prefix.length())
				{
					return NMT_HIT;
				}
				return NMT_GRAZED;
			}
		}
		else
		{
			if (command_name_prefix.substr(0, command_name.length()) == command_name)
			{
				return NMT_OVERSHOT;
			}
		}
		return NMT_MISSED;
	}

	bool CommandList::hasCommandsWhereCommandNameStartsWith(const CommandName& command_name_prefix) const noexcept
	{
		return true;
	}

	void CommandList::recursivelyApplyDefaultState() const
	{
		for (auto& _command : this->children)
		{
			SOUP_IF_UNLIKELY (!_command->isPhysical())
			{
				continue;
			}
			auto* command = (CommandPhysical*)_command.get();
			SOUP_IF_UNLIKELY (command->flags & CMDFLAG_STATE_AND_CHECK_FINISHLIST)
			{
				break;
			}
#if GUI_DEBUG
			g_gui.persistent_debug_text = std::string(command->getPathEn()).append(": [Default]");
#endif
			SOUP_IF_LIKELY (command->supportsSavedState())
			{
				__try
				{
					command->applyDefaultState();
				}
				__EXCEPTIONAL()
				{
				}
			}
			if (command->isListNonAction())
			{
				((CommandList*)command)->recursivelyApplyDefaultState();
			}
		}
#if GUI_DEBUG
		g_gui.persistent_debug_text = {};
#endif
	}

	void CommandList::recursivelySaveStateInMemory(std::unordered_map<std::string, std::string>& state, const std::string& prefix) const
	{
		for (const auto& _command : this->children)
		{
			SOUP_IF_UNLIKELY (!_command->isPhysical())
			{
				continue;
			}
			auto* command = (const CommandPhysical*)_command.get();
			if ((command->flags & CMDFLAG_TEMPORARY))
			{
				continue;
			}
			if (command->flags & CMDFLAG_STATE_AND_CHECK_FINISHLIST)
			{
				break;
			}
			if (command->supportsStateOperations()
				&& command->supportsSavedState()
				)
			{
				std::string command_state = command->getState();
				if (command_state != command->getDefaultState())
				{
					state.emplace(std::string(prefix).append(command->getNameForConfig()), command_state);
				}
			}
			if (command->isListNonAction())
			{
				std::string prefix_{ prefix };
				prefix_.append(command->getNameForConfig()).push_back('>');
				((CommandList*)command)->recursivelySaveStateInMemory(state, std::move(prefix_));
			}
		}
	}

	size_t CommandList::countChildren(bool include_invisible, bool include_non_resolvable) const noexcept
	{
		size_t count = children.size();
		if (!include_invisible && isT<CommandListConcealer>())
		{
			count -= as<CommandListConcealer>()->countInvisibleChildren();
		}
		if (!include_non_resolvable)
		{
			count -= dividers;
		}
		return count;
	}

	size_t CommandList::countVisibleChildren() const noexcept
	{
		if (isT<CommandListConcealer>())
		{
			return as<CommandListConcealer>()->countVisibleChildren();
		}
		return children.size();
	}

	std::wstring CommandList::getCursorText() const
	{
		const size_t num_children = countChildren(false, g_gui.cursor_pos_includes_dividers);
		if (num_children != 0)
		{
			auto str = fmt::to_wstring((g_gui.cursor_pos_includes_dividers ? m_cursor : getCursorIgnoreUnresolvable()) + 1);
			str.push_back(L'/');
			str.append(fmt::to_wstring(num_children));
			return str;
		}
		return {};
	}

	cursor_t CommandList::getCursorIgnoreUnresolvable() const
	{
		if (dividers == 0)
		{
			return m_cursor;
		}
		cursor_t theoretical_i = 0;
		cursor_t practical_i = 0;
		EXCEPTIONAL_LOCK_READ(g_gui.root_mtx)
		for (auto& child : children)
		{
			if (child->canBeResolved())
			{
				if (theoretical_i >= m_cursor)
				{
					break;
				}
				practical_i++;
			}
			theoretical_i++;
		}
		EXCEPTIONAL_UNLOCK_READ(g_gui.root_mtx)
		return practical_i;
	}

	size_t CommandList::countResolvableChildren() const noexcept
	{
		return children.size() - dividers;
	}

	void CommandList::forEachChild(std::function<void(Command*)>&& func) const
	{
		EXCEPTIONAL_LOCK_READ(g_gui.root_mtx)
		for (auto& command : this->children)
		{
			func(command.get());
		}
		EXCEPTIONAL_UNLOCK_READ(g_gui.root_mtx)
	}

	void CommandList::forEachPhysicalChild(const std::function<void(CommandPhysical*)>& func) const
	{
		EXCEPTIONAL_LOCK_READ(g_gui.root_mtx)
		for (auto& command : this->children)
		{
			if (command->isPhysical())
			{
				func((CommandPhysical*)command.get());
			}
		}
		EXCEPTIONAL_UNLOCK_READ(g_gui.root_mtx)
	}

	void CommandList::forEachChildAsPhysical(std::function<bool(CommandPhysical*)>&& consumer) const
	{
		EXCEPTIONAL_LOCK_READ(g_gui.root_mtx)
		for (auto& command : this->children)
		{
			if (!consumer(command->getPhysical()))
			{
				break;
			}
		}
		EXCEPTIONAL_UNLOCK_READ(g_gui.root_mtx)
	}

	void CommandList::recursiveForEachNonListChild(const std::function<void(Command*)>& func) const
	{
		EXCEPTIONAL_LOCK_READ(g_gui.root_mtx)
		recursiveForEachNonListChildRootReadLocked(func);
		EXCEPTIONAL_UNLOCK_READ(g_gui.root_mtx)
	}

	void CommandList::recursiveForEachNonListChildRootReadLocked(const std::function<void(Command*)>& func) const
	{
		for (auto& command : this->children)
		{
			if (command->isList())
			{
				((CommandList*)command.get())->recursiveForEachNonListChildRootReadLocked(func);
			}
			else
			{
				func(command.get());
			}
		}
	}

	void CommandList::recursiveForEachChild(const std::function<bool(const std::unique_ptr<Command>&)>& consumer) const
	{
		EXCEPTIONAL_LOCK_READ(g_gui.root_mtx)
		recursiveForEachChildImpl(consumer);
		EXCEPTIONAL_UNLOCK_READ(g_gui.root_mtx)
	}

	void CommandList::recursiveForEachChildImpl(const std::function<bool(const std::unique_ptr<Command>&)>& consumer) const
	{
		for (const auto& command : this->children)
		{
			if (consumer(command) && command->isList())
			{
				((CommandList*)command.get())->recursiveForEachChildImpl(consumer);
			}
		}
	}

	void CommandList::setCursorPos(const Command* child) const
	{
		EXCEPTIONAL_LOCK_READ(g_gui.root_mtx)
		const auto list = g_menu_grid.getColumn(child);
		if (list)
		{
			cursor_t draw_cursor = 0;
			auto i = children.begin() + m_offset + list->offset;
			while (i != children.end())
			{
				if (child == i->get())
				{
					auto pos_int = g_renderer.posH2C(float(list->x) + (float(g_renderer.command_width) * 0.5f), float(list->y) + (32.0f * (float(draw_cursor) + 0.5f)));
					POINT pos = { (int)pos_int.x, (int)pos_int.y };
					ClientToScreen(pointers::hwnd, &pos);
					SetCursorPos(pos.x, pos.y);
					break;
				}
				if (++draw_cursor == g_gui.getCommandsOnScreenLimit())
				{
					break;
				}
				++i;
			}
		}
		EXCEPTIONAL_UNLOCK_READ(g_gui.root_mtx)
	}

	void CommandList::sortChildren(sort_algo_t sort_algo)
	{
#ifdef STAND_DEBUG
		SOUP_ASSERT(g_gui.root_mtx.isWriteLockedByThisThread());
#endif
		std::sort(children.begin(), children.end(), sort_algo);
	}

	bool CommandList::sortAlgoName(const std::unique_ptr<Command>& a, const std::unique_ptr<Command>& b)
	{
		std::string a_name = a->getMenuName().getLocalisedUtf8();
		std::string b_name = b->getMenuName().getLocalisedUtf8();
		StringUtils::simplify(a_name);
		StringUtils::simplify(b_name);
		return alphanum_less(a_name, b_name);
	}
}
