#include "CommandPhysical.hpp"

#include <fmt/core.h>

#include "Click.hpp"
#include "CommandCtxHotkeys.hpp"
#include "CommandDivider.hpp"
#include "CommandExtraInfo.hpp"
#include "CommandLink.hpp"
#include "CommandList.hpp"
#include "CommandListActionItem.hpp"
#include "CommandListConcealer.hpp"
#include "CommandListSelect.hpp"
#include "CommandLua.hpp"
#include "CommandReadonlyLink.hpp"
#include "CommandSlider.hpp"
#include "CommandToggle.hpp"
#include "ContextMenu.hpp"
#include "ExecCtx.hpp"
#include "FiberPool.hpp"
#include "Gui.hpp"
#include "MenuGrid.hpp"
#include "regular_event.hpp"
#include "StringUtils.hpp"
#include "Util.hpp"
#include "validate_file_name.hpp"
#include "Worker.hpp"

namespace Stand
{
	CommandPhysical::CommandPhysical(const CommandType type, CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, const commandflags_t flags, const CommandPerm perm, const std::vector<Hotkey>& default_hotkeys)
		: CommandIssuable(parent, std::move(command_names), perm, flags, type), menu_name(std::move(menu_name)), help_text(std::move(help_text)), hotkeys(default_hotkeys)
	{
	}

	void CommandPhysical::onAttach() const noexcept
	{
		SOUP_IF_UNLIKELY (type == COMMAND_DIVIDER)
		{
			++parent->dividers;
		}
	}

	void CommandPhysical::preDelete()
	{
		CommandIssuable::preDelete();

		removeFromCommandsWithHotkeys();

		auto* focus = g_gui.getCurrentMenuFocus();
		if (focus != nullptr && focus->isT<CommandLink>() && focus->as<CommandLink>()->getTarget() == this)
		{
			g_menu_grid.update();
		}
	}

	void CommandPhysical::preDetach()
	{
		CommandIssuable::preDetach();

		if (isConcealed())
		{
			parent->as<CommandListConcealer>()->setVisibleRaw(this);
		}

		if (ContextMenu::isOpen() && g_gui.getCurrentMenuFocus() == this)
		{
			ContextMenu::close(TC_OTHER);
		}
	}

	const Label& CommandPhysical::getMenuName() const
	{
		return menu_name;
	}

	void CommandPhysical::setMenuName(Label&& menu_name)
	{
		if (this->menu_name != menu_name)
		{
			setMenuNameNoWeb(std::move(menu_name));
			updateWebState();
		}
	}

	void CommandPhysical::setMenuNameNoWeb(Label&& menu_name)
	{
		this->menu_name = std::move(menu_name);
		processVisualUpdate();
	}

	void CommandPhysical::updateWebState() const
	{
		if (parent != nullptr)
		{
			parent->updateWebState();
		}
	}

	void CommandPhysical::setHelpText(const Label& help_text)
	{
		if (this->help_text != help_text)
		{
			this->help_text = help_text;
			processVisualUpdate();
			updateHelpTextOnWeb();
		}
	}

	void CommandPhysical::setHelpText(Label&& help_text)
	{
		if (this->help_text != help_text)
		{
			this->help_text = std::move(help_text);
			processVisualUpdate();
			updateHelpTextOnWeb();
		}
	}

	void CommandPhysical::updateHelpTextOnWeb() const
	{
		updateWebState();
	}

	bool CommandPhysical::isInViewport() const
	{
		if (g_gui.opened)
		{
			if (auto* active_list = g_gui.getCurrentUiList())
			{
				return active_list->isInViewport(this);
			}
		}
		return false;
	}

	bool CommandPhysical::isFocused() const
	{
		if (auto active_list = g_gui.getCurrentUiList())
		{
			if (auto focus = active_list->getFocus())
			{
				return focus->getPhysical() == this;
			}
		}
		return false;
	}

	void CommandPhysical::processVisualUpdate(bool draw_data_only) const
	{
		if (!g_menu_grid.content_update_queued
			&& g_gui.isListVisible()
			)
		{
			EXCEPTIONAL_LOCK_READ(g_gui.root_mtx)
			auto* active_list = g_gui.getCurrentUiList();
			if (active_list != nullptr)
			{
				if (active_list->isInViewportRootReadlocked(this))
				{
					bool full_update = false;
					if (!draw_data_only)
					{
						if (auto focus = active_list->getFocus())
						{
							if (focus->getPhysical() == this)
							{
								full_update = true;
							}
						}
					}
					if (full_update)
					{
						g_menu_grid.update();
					}
					else
					{
						g_menu_grid.updateList(this);
					}
				}
				else if (isList())
				{
					do
					{
						if (active_list == this)
						{
							g_menu_grid.updateAddress();
							break;
						}
						active_list = active_list->parent;
					} while (active_list != nullptr);
				}
			}
			EXCEPTIONAL_UNLOCK_READ(g_gui.root_mtx)
		}
	}

	void CommandPhysical::setCommandNames(std::vector<CommandName>&& command_names)
	{
		this->command_names = std::move(command_names);
		processVisualUpdate(false);
		updateWebState();
	}

	std::vector<CommandName> CommandPhysical::getCommandNamesFromMenuName(const Label& menu_name)
	{
		auto str = menu_name.getEnglishForCommandName();
		auto sep = str.find(CMDNAME(" ("));
		if (sep != CommandName::npos)
		{
			str = str.erase(sep, str.length() - sep);
		}
		StringUtils::simplify(str);
		return { std::move(str) };
	}

	void CommandPhysical::useCommandNamesFromMenuName()
	{
		command_names = getCommandNamesFromMenuName(menu_name);
	}

	Label CommandPhysical::getActivationName() const
	{
		if (!menu_name.isLiteral())
		{
			switch (menu_name.getLocalisationHash())
			{
			case ATSTRINGHASH("ON"):
			case ATSTRINGHASH("DOFF"):
			case ATSTRINGHASH("CALLVEH"):
			case ATSTRINGHASH("VEH_ENT"):
			case ATSTRINGHASH("B_CB"):
			case ATSTRINGHASH("HOTKEY"):
			case ATSTRINGHASH("ALLGNS"):
			case ATSTRINGHASH("OPQ"):
				return getActivationNameImplCombineWithParent(": ");
			}
		}
		return menu_name;
	}

	Label CommandPhysical::getActivationNameImplCombineWithParent(const char* joiner) const
	{
		return LIT(std::move(std::string(parent->menu_name.getLocalisedUtf8()).append(joiner).append(menu_name.getLocalisedUtf8())));
	}

	bool CommandPhysical::hasHelpTextForListing() const
	{
		return !help_text.empty() && help_text.getLocalisationHash() != ATSTRINGHASH("C2C");
	}

	bool CommandPhysical::isSelectable() const noexcept
	{
		return (
			(type != COMMAND_DIVIDER || CommandDivider::selectable)
			&& (!isSlider() || as<CommandSlider>()->isSelectable())
		);
	}

	std::wstring CommandPhysical::getCommandSyntax() const
	{
		if (command_names.empty())
		{
			return {};
		}
		return getCommandSyntaxImpl(cmdNameToUtf16(command_names.at(0)));
	}

	std::wstring CommandPhysical::getCommandSyntaxImpl(const std::wstring& syntax)
	{
		std::wstring str{ LANG_GET_W("CMD") };
		str.append(L": ");
		str.append(syntax);
		return str;
	}

	bool CommandPhysical::canBeUsedByOtherPlayers() const
	{
		return perm != COMMANDPERM_USERONLY && !command_names.empty();
	}

	bool CommandPhysical::isHelpTextShown() const
	{
		return (g_gui.show_help_text || type == COMMAND_READONLY_NAME || type == COMMAND_READONLY_LINK);
	}

	void CommandPhysical::populateCorner(std::vector<std::wstring>& corner) const
	{
		if (!help_text.empty() && isHelpTextShown())
		{
			corner.emplace_back(help_text.getLocalisedUtf16());
		}

		if (g_gui.show_syntax)
		{
			std::wstring syntax = getCommandSyntax();
			if (!syntax.empty())
			{
				corner.push_back(std::move(syntax));
			}
		}

		if (isSlider())
		{
			if (g_gui.show_slider_behaviour)
			{
				if (as<CommandSlider>()->is_click_to_apply)
				{
					corner.emplace_back(LANG_GET_W("C2APPLY"));
				}
				else
				{
					if (!command_names.empty())
					{
						if (g_gui.command_box_enabled)
						{
							corner.emplace_back(LANG_GET_W("C2IN"));
						}
					}
					else
					{
						corner.emplace_back(as<CommandSlider>()->getRangeString());
					}
				}
			}
		}
		else switch (type)
		{
		case COMMAND_TOGGLE:
			if (as<CommandToggle>()->correlation.isActive())
			{
				corner.emplace_back(StringUtils::utf8_to_utf16(as<CommandToggle>()->correlation.getExplanation()));
			}
			break;

		case COMMAND_READONLY_NAME:
			corner.emplace_back(LANG_GET_W("C2CN"));
			break;

		case COMMAND_READONLY_LINK:
			if (!as<CommandReadonlyLink>()->link.empty())
			{
				corner.emplace_back(StringUtils::utf8_to_utf16(LANG_FMT("C2OL", fmt::arg("link", as<CommandReadonlyLink>()->link))));
			}
			break;

		case COMMAND_LIST_SELECT:
			if (g_gui.show_slider_behaviour)
			{
				corner.emplace_back(LANG_GET_W("C2SEL"));
			}
			auto value_help_text = as<CommandListSelect>()->getCurrentValueHelpText();
			if (!value_help_text.empty())
			{
				corner.emplace_back(std::move(((const CommandListSelect*)this)->getCurrentValueMenuName().getLocalisedUtf16().append(L": ").append(value_help_text.getLocalisedUtf16())));
			}
			break;
		}

		if (canBeUsedByOtherPlayers() && g_gui.show_nonuseronly)
		{
			corner.emplace_back(LANG_FMT_W("FTRLST_OTHP", getCommandPermLabel(perm).getLocalisedUtf16()));
		}

		if (g_gui.show_cmd_owner
			&& hasLuaData()
			)
		{
			auto data = getLuaData();
			bool ownership_is_clear = false;
			if (hasParent(data->script))
			{
				// Ownership is clear if we're within a script's root.
				ownership_is_clear = true;
			}
			else
			{
				// Or if we're within a list where we already showed ownership.
				for (auto list = parent; list; list = list->parent)
				{
					if (list->hasLuaData()
						//&& list->getLuaData()->script == data->script
						)
					{
						ownership_is_clear = true;
						break;
					}
				}
			}
			if (!ownership_is_clear)
			{
				corner.emplace_back(LANG_FMT_W("SHWCMDOWNR_T", data->script->menu_name.getLiteralUtf16()));
			}
		}
	}

	bool CommandPhysical::canShowSliderRangeInFeatureList() const
	{
		return supportsStateOperations();
	}

	std::string CommandPhysical::getFeatureListExtraText() const
	{
		std::string extra_text = {};
		if (hasHelpTextForListing())
		{
			extra_text.append((const char*)u8" — ").append(help_text.getLocalisedUtf8());
		}
		std::wstring syntax = {};
		if (!isSlider() || canShowSliderRangeInFeatureList())
		{
			syntax = this->getCommandSyntax();
		}
		else
		{
			syntax = CommandPhysical::getCommandSyntax();
		}
		if (!syntax.empty())
		{
			extra_text.append((const char*)u8" — ").append(StringUtils::utf16_to_utf8(std::move(syntax)));
		}
		return extra_text;
	}

	bool CommandPhysical::isActiveOnWeb() const
	{
		if (!g_gui.isUnloadPending())
		{
			if (isListNonAction())
			{
				return g_gui.web_focus == this;
			}
			return g_gui.web_focus == parent;
		}
		return false;
	}

	CommandPhysical* CommandPhysical::getStateCommand()
	{
		CommandPhysical* stateCommand = this;
		if (!stateCommand->supportsUserStateOperations())
		{
			do
			{
				stateCommand = stateCommand->parent;
				if (stateCommand == nullptr)
				{
					break;
				}
			} while (!stateCommand->supportsUserStateOperations());

			if (stateCommand != nullptr)
			{
				if (stateCommand->flags & CMDFLAG_NOT_STATE_FOR_CHILDREN)
				{
					stateCommand = nullptr;
				}
			}
		}
		return stateCommand;
	}

	void CommandPhysical::ensurePhysicalFocus(ThreadContext thread_context)
	{
		auto* focus = g_gui.getCurrentMenuFocus();
		if (focus != this && focus->getPhysical() == this)
		{
			this->focus(thread_context);
		}
	}

	void CommandPhysical::onFocus(ThreadContext thread_context, Direction momentum)
	{
	}

	void CommandPhysical::onBlur(ThreadContext thread_context, Direction momentum)
	{
	}

	void CommandPhysical::onTickInGameViewport()
	{
	}

	void CommandPhysical::onTickInWebViewport()
	{
	}

	bool CommandPhysical::receivesOnTickInGameViewportThisTick() const
	{
		return isInViewport();
	}

	bool CommandPhysical::receivesOnTickInWebViewportThisTick() const
	{
		return g_gui.web_focus == parent && parent->canDispatchOnTickInWebViewportForChildren();
	}

	void CommandPhysical::onTickInWebViewportImplRedirect()
	{
		dispatchOnTickInGameViewportIfNotThisTick();
	}

	void CommandPhysical::dispatchOnTickInGameViewportIfNotThisTick()
	{
		if (!receivesOnTickInGameViewportThisTick())
		{
			onTickInGameViewport();
		}
	}

	void CommandPhysical::onPreScriptedAccess()
	{
	}

	void CommandPhysical::onClick(Click& click)
	{
		if (!command_names.empty())
		{
			std::wstring prefill{ cmdNameToUtf16(command_names.at(0)) };
			prefill.push_back(L' ');
			click.showCommandBoxIfPossible(std::move(prefill));
		}
	}

	void CommandPhysical::getExtraInfo(CommandExtraInfo& info, std::wstring& args)
	{
		info.completed_hint = getActivationName().getLocalisedUtf16();
		if (!help_text.empty())
		{
			info.completed_hint.append(L" - ");
			info.completed_hint.append(help_text.getLocalisedUtf16());
		}
	}

	void CommandPhysical::onCommand(Click& click, std::wstring& args)
	{
		onClick(click);
	}

	bool CommandPhysical::checkArgsLength(Click& click, const std::wstring& args, const size_t max)
	{
		if (args.length() > max)
		{
			click.setResponse(LIT(LANG_FMT("CMDLEN", max)));
			onCommandReopen(click, args);
			return false;
		}
		return true;
	}

	bool CommandPhysical::onCommandValidateFileName(Click& click, std::wstring& args)
	{
		if (args.empty())
		{
			onClick(click);
			return false;
		}
		if (validate_file_name(args, true))
		{
			return true;
		}
		click.setResponse(LOC("BADFILE"));
		onCommandReopen(click, args);
		args.clear();
		return false;
	}

	void CommandPhysical::onCommandInvalidArg(Click& click, const std::wstring& arg) const
	{
		click.setResponse(LOC("INVARG"));
		onCommandReopen(click, arg);
	}

	void CommandPhysical::onCommandReopen(Click& click, const std::wstring& args) const
	{
		if (click.canShowCommandBox())
		{
			std::wstring prefill{ cmdNameToUtf16(command_names.at(0)) };
			prefill.push_back(L' ');
			prefill.append(args);
			click.showCommandBox(std::move(prefill));
		}
	}

	bool CommandPhysical::onLeft(Click& click, bool holding)
	{
		return true;
	}

	bool CommandPhysical::onRight(Click& click, bool holding)
	{
		return true;
	}

	void CommandPhysical::loadState(ClickType type)
	{
		FiberPool::queueJob([this, type]
		{
			auto entry = g_gui.active_profile.data.find(this->getPathConfig());
			if (entry != g_gui.active_profile.data.end())
			{
				Click click(type, TC_SCRIPT_YIELDABLE);
				this->setState(click, entry->second);
			}
			else
			{
				this->applyDefaultState();
			}
		});
	}

	std::string CommandPhysical::getNameForConfig() const
	{
		return menu_name.getEnglishUtf8();
	}

	std::string CommandPhysical::getState() const
	{
		return {};
	}

	std::string CommandPhysical::getDefaultState() const
	{
		return {};
	}

	void CommandPhysical::setState(Click& click, const std::string& state)
	{
	}

	void CommandPhysical::applyDefaultState()
	{
	}

	void CommandPhysical::saveStateInMemory()
	{
		auto state = getState();
		if (state == getDefaultState())
		{
			g_gui.active_profile.data.erase(getPathConfig());
		}
		else
		{
			g_gui.active_profile.data[getPathConfig()] = std::move(state);
		}
	}

	void CommandPhysical::processStateChange()
	{
		if (g_gui.isUsingAutosaveState())
		{
			saveStateInMemory();
		}

		g_gui.on_command_state_change();
	}

	bool CommandPhysical::isOpenInContextMenu() const noexcept
	{
		return ContextMenu::isOpen() && ContextMenu::getTargetPhysical() == this;
	}

	static std::vector<Hotkey> s_no_hotkeys;

	const std::vector<Hotkey>& CommandPhysical::getDefaultHotkeys() const noexcept
	{
		return s_no_hotkeys;
	}

	bool CommandPhysical::canCountAsCommandWithHotkeys() const noexcept
	{
		return getDefaultHotkeys().empty();
	}

	bool CommandPhysical::canBeTriggeredInCurrentState() const
	{
		return this->parent->type != COMMAND_LIST_SELECT
			|| ((CommandListSelect*)this->parent)->value != ((CommandListActionItem*)this)->value;
	}

	bool CommandPhysical::isInDefaultHotkeyState() const
	{
		return hotkeys == getDefaultHotkeys();
	}

	void CommandPhysical::applyDefaultHotkeys()
	{
		hotkeys = getDefaultHotkeys();
	}

	bool CommandPhysical::canHotkeyBeRemoved(const Hotkey hotkey) const noexcept
	{
		return getDefaultHotkeys().empty() || hotkeys.size() > 1;
	}

	bool CommandPhysical::hasHotkey(const Hotkey hotkey) const noexcept
	{
		return std::find(hotkeys.begin(), hotkeys.end(), hotkey) != hotkeys.end();
	}

	void CommandPhysical::modifyHotkeys(bool hotkey_add, const Hotkey hotkey, ClickType type)
	{
		if (!hasHotkey(hotkey))
		{
			if (canCountAsCommandWithHotkeys() && hotkeys.empty())
			{
				EXCEPTIONAL_LOCK(g_gui.commands_with_hotkeys_mtx)
				g_gui.commands_with_hotkeys.emplace_back(this);
				EXCEPTIONAL_UNLOCK(g_gui.commands_with_hotkeys_mtx)
			}
			if (hotkey_add)
			{
				hotkeys.emplace_back(hotkey);
			}
			else
			{
				hotkeys = { hotkey };
			}
			processVisualUpdate(false);
			onHotkeysChanged(type);
			updateHotkeysInContextMenu();
			updateHotkeysState();
			g_gui.hotkeys.save();
			if (type == CLICK_MENU)
			{
				Util::toast(LANG_FMT("HOTKEY_T", getActivationName().getLocalisedUtf8()), TOAST_ABOVE_MAP);
			}
		}
	}

	void CommandPhysical::updateHotkeysState() const
	{
		g_gui.hotkeys.data[getPathConfig()] = hotkeys;
	}

	void CommandPhysical::checkAddToCommandsWithHotkeys()
	{
		if (canCountAsCommandWithHotkeys() && !hotkeys.empty())
		{
			EXCEPTIONAL_LOCK(g_gui.commands_with_hotkeys_mtx)
			g_gui.commands_with_hotkeys.emplace_back(this);
			EXCEPTIONAL_UNLOCK(g_gui.commands_with_hotkeys_mtx)
		}
	}

	void CommandPhysical::removeFromCommandsWithHotkeys() const noexcept
	{
		EXCEPTIONAL_LOCK(g_gui.commands_with_hotkeys_mtx)
		auto i = std::find(g_gui.commands_with_hotkeys.begin(), g_gui.commands_with_hotkeys.end(), this);
		if (i != g_gui.commands_with_hotkeys.end())
		{
			g_gui.commands_with_hotkeys.erase(i);
		}
		EXCEPTIONAL_UNLOCK(g_gui.commands_with_hotkeys_mtx)
	}

	void CommandPhysical::removeHotkey(const Hotkey hotkey, ClickType type)
	{
		for (auto i = hotkeys.begin(); i != hotkeys.end(); i++)
		{
			if (*i == hotkey)
			{
				i = hotkeys.erase(i);
				break;
			}
		}
		processVisualUpdate(false);
		g_gui.hotkeys.data[getPathConfig()] = hotkeys;
		onHotkeysChanged(type);
		updateHotkeysInContextMenu();
	}

	void CommandPhysical::onHotkeysChanged(ClickType type) const
	{
	}

	void CommandPhysical::updateHotkeysInContextMenu() const
	{
		if (isOpenInContextMenu())
		{
			auto* list = ContextMenu::getHotkeysList();
			if (list->isThisOrSublistCurrentUiListInTab())
			{
				list->update();
			}
		}
	}

	void CommandPhysical::queueJob(std::function<void()>&& func)
	{
		if (!job_queued)
		{
			job_queued = true;
			FiberPool::queueJob([this, func{ std::move(func) }]
			{
				job_queued = false;
				func();
			});
		}
	}

	void CommandPhysical::queueJob(std::function<void(ThreadContext)>&& func)
	{
		if (!job_queued)
		{
			job_queued = true;
			FiberPool::queueJob([this, func{ std::move(func) }]
			{
				job_queued = false;
				func(TC_SCRIPT_YIELDABLE);
			});
		}
	}

	void CommandPhysical::ensureYieldableScriptThread(ThreadContext thread_context, std::function<void()>&& func)
	{
		if (thread_context == TC_SCRIPT_YIELDABLE)
		{
			func();
		}
		else
		{
			queueJob(std::move(func));
		}
	}

	void CommandPhysical::ensureYieldableScriptThread(const Click& click, std::function<void()>&& func)
	{
		ensureYieldableScriptThread(click.thread_context, std::move(func));
	}

	void CommandPhysical::ensureYieldableScriptThread(std::function<void()>&& func)
	{
		ensureYieldableScriptThread(ExecCtx::get().tc, std::move(func));
	}

	void CommandPhysical::ensureGameTls(std::function<void()>&& func)
	{
		if (thread_context_has_game_tls(ExecCtx::get().tc))
		{
			func();
		}
		else
		{
			queueJob(std::move(func));
		}
	}

	void CommandPhysical::ensureScriptThread(ThreadContext thread_context, std::function<void()>&& func)
	{
		if (thread_context_is_script(thread_context))
		{
			func();
		}
		else
		{
			queueJob(std::move(func));
		}
	}

	void CommandPhysical::ensureScriptThread(std::function<void()>&& func)
	{
		ensureScriptThread(ExecCtx::get().tc, std::move(func));
	}

	void CommandPhysical::ensureScriptThread(const Click& click, std::function<void()>&& func)
	{
		ensureScriptThread(click.thread_context, std::move(func));
	}

	void CommandPhysical::ensureScriptThread(Click& click, std::function<void(Click&)>&& func)
	{
		if (!job_queued)
		{
			job_queued = true;
			click.ensureScriptThread([this, func{ std::move(func) }](Click& click)
			{
				job_queued = false;
				func(click);
			});
		}
	}

	void CommandPhysical::ensureScriptThread(ThreadContext thread_context, std::function<void(ThreadContext)>&& func)
	{
		if (thread_context_is_script(thread_context))
		{
			func(thread_context);
		}
		else
		{
			queueJob(std::move(func));
		}
	}

	void CommandPhysical::ensureScriptThread(const Click& click, std::function<void(ThreadContext)>&& func)
	{
		ensureScriptThread(click.thread_context, std::move(func));
	}

	void CommandPhysical::ensureScriptThread(ThreadContext thread_context, bool root_readlocked, std::function<void(ThreadContext, bool)>&& func)
	{
		if (thread_context_is_script(thread_context))
		{
			func(thread_context, root_readlocked);
		}
		else
		{
			queueJob([func{ std::move(func) }]
			{
				func(TC_SCRIPT_YIELDABLE, false);
			});
		}
	}

	void CommandPhysical::ensureScriptThread(const Click& click, bool root_readlocked, std::function<void(ThreadContext, bool)>&& func)
	{
		ensureScriptThread(click.thread_context, root_readlocked, std::move(func));
	}

	void CommandPhysical::queueWorkerJob(std::function<void()>&& func)
	{
		if (!job_queued)
		{
			job_queued = true;
			Worker::queueJob([this, func{ std::move(func) }]
			{
				job_queued = false;
				func();
			});
		}
	}

	void CommandPhysical::ensureWorkerContext(ThreadContext thread_context, std::function<void()>&& func)
	{
		if (thread_context == TC_WORKER)
		{
			func();
		}
		else
		{
			queueWorkerJob(std::move(func));
		}
	}

	void CommandPhysical::ensureWorkerContext(const Click& click, std::function<void()>&& func)
	{
		ensureWorkerContext(click.thread_context, std::move(func));
	}

	/*CommandResponse CommandPhysical::ensureScriptThread(const Click& click, std::function<CommandResponse()>&& func, CommandResponse&& default_return)
	{
		if (click.thread_context)
		{
			return func();
		}
		if (!job_queued)
		{
			job_queued = true;
			fiber_pool::queueJob([this, func{ std::move(func) }]
				{
					job_queued = false;
					func();
				});
		}
		return default_return;
	}*/

	void CommandPhysical::registerScriptTickEventHandlerInContext(std::function<bool()>&& handler)
	{
		reScriptTickEvent::registerHandlerInThreadContext(std::move(handler), menu_name.getEnglishUtf8());
	}

	void CommandPhysical::registerScriptTickEventHandler(ThreadContext thread_context, std::function<bool()>&& handler)
	{
		ensureScriptThread(thread_context, [this, handler{ std::move(handler) }]() mutable
		{
			registerScriptTickEventHandlerInContext(std::move(handler));
		});
	}

	void CommandPhysical::registerScriptTickEventHandler(const Click& click, std::function<bool()>&& handler)
	{
		registerScriptTickEventHandler(click.thread_context, std::move(handler));
	}

	void CommandPhysical::registerScriptTickEventHandler(std::function<bool()>&& handler)
	{
		ThreadContext tc = ExecCtx::get().tc;
		if (ExecCtx::get().script_tick_event_mtx_is_locked)
		{
			tc = TC_OTHER; // Force queueJob to avoid deadlock
		}
		registerScriptTickEventHandler(tc, std::move(handler));
	}

	void CommandPhysical::registerAsyncTickEventHandler(ThreadContext thread_context, std::function<bool()>&& handler)
	{
		ensureScriptThread(thread_context, [this, handler{ std::move(handler) }]() mutable
		{
			reScriptTickEvent::registerHandlerInThreadContext(std::move(handler), menu_name.getEnglishUtf8());
		});
	}

	void CommandPhysical::registerAsyncTickEventHandler(const Click& click, std::function<bool()>&& handler)
	{
		registerAsyncTickEventHandler(click.thread_context, std::move(handler));
	}

	void CommandPhysical::registerPostTickEventHandler(std::function<bool()>&& handler)
	{
		rePostTickEvent::registerHandler(std::move(handler), menu_name.getEnglishUtf8());
	}

	void CommandPhysical::registerPresentEventHandler(std::function<bool()>&& handler)
	{
		rePresentEvent::registerHandler(std::move(handler), menu_name.getEnglishUtf8());
	}
}
