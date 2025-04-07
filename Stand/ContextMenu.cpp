#include "ContextMenu.hpp"

#include "CommandCtxAddress.hpp"
#include "CommandCtxHotkeys.hpp"
#include "CommandCtxStar.hpp"
#include "CommandCtxToggleCorrelation.hpp"
#include "CommandCtxToggleCorrelationInvert.hpp"
#include "CommandLambdaAction.hpp"
#include "CommandQuickCtx.hpp"
#include "CommandSlider.hpp"
#include "ExecCtx.hpp"
#include "FiberPool.hpp"
#include "fmt_arg.hpp"
#include "Gui.hpp"
#include "StringUtils.hpp"
#include "Tutorial.hpp"

namespace Stand
{
	Command* ContextMenu::getTarget() noexcept
	{
		return g_gui.getCurrentMenuFocus();
	}

	CommandPhysical* ContextMenu::getTargetPhysical() noexcept
	{
		return g_gui.getCurrentMenuFocusPhysical();
	}

	bool ContextMenu::isAvailable() noexcept
	{
		return Tutorial::state >= TUT_DONE;
	}

	void ContextMenu::toggleIfAvailable(ThreadContext thread_context)
	{
		if (isAvailable())
		{
			toggle(thread_context);
		}
	}

	void ContextMenu::toggle(ThreadContext thread_context)
	{
		if (isOpen())
		{
			close(thread_context);
		}
		else
		{
			open(thread_context);
		}
		g_gui.sfxOpenClose(thread_context, isOpen());
	}

	bool ContextMenu::isOpen() noexcept
	{
		return root.operator bool();
	}

	void ContextMenu::close(ThreadContext thread_context)
	{
		g_gui.updateFocus(thread_context, TELEPORT);
		root.reset();
		CommandList* list = g_gui.m_active_list.at(g_gui.root_cursor);
		list->fixCursorAndOffset();
		list->onActiveListUpdate();
	}

	void ContextMenu::open(ThreadContext thread_context)
	{
		if (auto* const target = getTargetPhysical())
		{
			root = std::make_unique<CommandList>(nullptr, Label(target->menu_name));
			view = root.get();

			// State
			auto stateCommand = target->getStateCommand();
			if (stateCommand != nullptr)
			{
				if (stateCommand == target && target->type == COMMAND_TOGGLE)
				{
					auto autostate = root->createChild<CommandCtxToggleCorrelation>((CommandToggle*)target);
					root->createChild<CommandCtxToggleCorrelationInvert>((CommandToggle*)target, autostate);
				}
				if (g_gui.active_profile.isInitialised()
					&& !(stateCommand->flags & CMDFLAG_TEMPORARY)
					&& stateCommand->supportsSavedState()
					)
				{
					if (!g_gui.isUsingAutosaveState())
					{
						root->createChild<CommandLambdaAction>(LOC("SSTATE"), {}, LOC("SSTATE_H_CTX2"), [](Click& click)
						{
							doSaveState(click);
						}, CMDFLAGS_ACTION, COMMANDPERM_USERONLY, CommandQuickCtx::instance->save_state->hotkeys);
					}
					root->createChild<CommandLambdaAction>(LOC("LSTATE"), {}, LOC("LSTATE_H_CTX2"), [](Click& click)
					{
						doLoadState(click);
					}, CMDFLAGS_ACTION, COMMANDPERM_USERONLY, CommandQuickCtx::instance->load_state->hotkeys);
				}
				root->createChild<CommandLambdaAction>(LOC("GODFLT"), ((target->isSlider() || target->isToggle()) && !target->command_names.empty())
					? std::vector<CommandName>(1, std::move(CommandName(target->command_names.at(0)).append(CMDNAME(" default"))))
					: CMDNAMES(), NOLABEL, [](Click& click)
				{
					doApplyDefaultState(click);
				}, CMDFLAGS_ACTION, COMMANDPERM_USERONLY, CommandQuickCtx::instance->apply_default_state->hotkeys);
			}

			// List - Children State
			if (target->isListNonAction())
			{
				root->createChild<CommandLambdaAction>(LOC("RGODFLT"), {}, NOLABEL, [](Click& click)
				{
					doApplyDefaultStateToChildren(click);
				}, CMDFLAGS_ACTION, COMMANDPERM_USERONLY, CommandQuickCtx::instance->apply_default_state_to_children->hotkeys);
			}

			// Slider Min/Max Value
			if (target->isT<CommandSlider>())
			{
				root->createChild<CommandLambdaAction>(LOC("CTX_MIN"), target->command_names.empty() ? CMDNAMES()
					: std::vector<CommandName>(1, std::move(CommandName(target->command_names.at(0)).append(CMDNAME(" min")))), NOLABEL, [](Click& click)
				{
					doMin(click);
				}, CMDFLAGS_ACTION, COMMANDPERM_USERONLY, CommandQuickCtx::instance->min->hotkeys);
				root->createChild<CommandLambdaAction>(LOC("CTX_MAX"), target->command_names.empty() ? CMDNAMES()
					: std::vector<CommandName>(1, std::move(CommandName(target->command_names.at(0)).append(CMDNAME(" max")))), NOLABEL, [](Click& click)
				{
					doMax(click);
				}, CMDFLAGS_ACTION, COMMANDPERM_USERONLY, CommandQuickCtx::instance->max->hotkeys);
			}

			if (!(target->flags & CMDFLAG_TEMPORARY)
				&& target->canBeResolved()
				)
			{
				root->createChild<CommandCtxHotkeys>();

				CommandList* stand_tab = g_gui.getStandTab();
				if (stand_tab != nullptr)
				{
					Command* stars = stand_tab->recursivelyResolveChildByMenuName(LOC("CMDSTARS"));
					if (stars != nullptr)
					{
						root->createChild<CommandCtxStar>(stars);
					}
				}
			}

			// Misc
			root->createChild<CommandCtxAddress>();

			SOUP_IF_UNLIKELY (Tutorial::state == TUT_CTX)
			{
				Tutorial::setState(TUT_DONE);
				g_gui.user_understands_context_menu = true;
				g_gui.saveTutorialFlags();
			}

			g_gui.updateActiveFocus(thread_context, TELEPORT, target);
		}

		CommandList* list = g_gui.m_active_list.at(g_gui.root_cursor);
		list->onActiveListUpdate();
	}

	CommandCtxHotkeys* ContextMenu::getHotkeysList()
	{
		return (CommandCtxHotkeys*)root->resolveChildByMenuName(LOC("HOTKEYS"));
	}

	void ContextMenu::openIntoHotkeysList(ThreadContext thread_context)
	{
		open(thread_context);
		CommandCtxHotkeys* const hotkeys = getHotkeysList();
		hotkeys->open(thread_context);
		hotkeys->close_context_menu_on_back = true;
	}

	void ContextMenu::doSaveState(Click& click)
	{
		if (!g_gui.active_profile.isInitialised())
		{
			return;
		}
		if (g_gui.isUsingAutosaveState())
		{
			click.setResponse(LOC("AUTOSAVE2_T"));
			return;
		}
		if (auto targetPhysical = getTargetPhysical())
		{
			if (auto stateCommand = targetPhysical->getStateCommand())
			{
				if (!(stateCommand->flags & CMDFLAG_TEMPORARY)
					&& stateCommand->supportsSavedState()
					)
				{
					click.setResponse(LIT(LANG_FMT("SSTATE_T_CTX",
						FMT_ARG("profile", g_gui.getActiveStateNameUtf8()),
						FMT_ARG("command", stateCommand->menu_name.getLocalisedUtf8())
					)));
					auto state = stateCommand->getState();
					if (state != stateCommand->getDefaultState())
					{
						g_gui.active_profile.data[stateCommand->getPathConfig()] = std::move(state);
						g_gui.active_profile.save();
					}
					else
					{
						auto ent = g_gui.active_profile.data.find(stateCommand->getPathConfig());
						if (ent != g_gui.active_profile.data.end())
						{
							g_gui.active_profile.data.erase(ent);
							g_gui.active_profile.save();
						}
					}
					return;
				}
			}
		}
		click.setResponse(LIT(LANG_FMT("CTX_UNAVAIL",
			FMT_ARG("option", LANG_GET("SSTATE")),
			FMT_ARG("command", getTarget()->getMenuName().getLocalisedUtf8())
		)));
	}

	void ContextMenu::doLoadState(Click& click)
	{
		if (!g_gui.active_profile.isInitialised())
		{
			return;
		}
		if (auto targetPhysical = getTargetPhysical())
		{
			if (auto stateCommand = targetPhysical->getStateCommand())
			{
				if (!(stateCommand->flags & CMDFLAG_TEMPORARY)
					&& stateCommand->supportsSavedState()
					)
				{
					g_gui.loadStateToMemory(g_gui.active_profile);
					click.setResponse(LIT(LANG_FMT("LSTATE_T_CTX2",
						FMT_ARG("profile", g_gui.getActiveStateNameUtf8()),
						FMT_ARG("command", stateCommand->menu_name.getLocalisedUtf8())
					)));
					stateCommand->loadState(click.type);
					return;
				}
			}
		}
		click.setResponse(LIT(LANG_FMT("CTX_UNAVAIL",
			FMT_ARG("option", LANG_GET("LSTATE")),
			FMT_ARG("command", getTarget()->getMenuName().getLocalisedUtf8())
		)));
	}

	void ContextMenu::doApplyDefaultState(Click& click)
	{
		if (auto targetPhysical = getTargetPhysical())
		{
			if (auto stateCommand = targetPhysical->getStateCommand())
			{
				click.setResponse(LIT(LANG_FMT("GODFLT_CTX", stateCommand->menu_name.getLocalisedUtf8())));
				ExecCtx::get().ensureScript([stateCommand]
				{
					stateCommand->applyDefaultState();
				});
				return;
			}
		}
		click.setResponse(LIT(LANG_FMT("CTX_UNAVAIL",
			FMT_ARG("option", LANG_GET("GODFLT")),
			FMT_ARG("command", getTarget()->getMenuName().getLocalisedUtf8())
		)));
	}

	void ContextMenu::doApplyDefaultStateToChildren(Click& click)
	{
		if (auto targetPhysical = getTargetPhysical())
		{
			if (targetPhysical->isT<CommandList>())
			{
				click.setResponse(LIT(LANG_FMT("RGODFLT_CTX", targetPhysical->menu_name.getLocalisedUtf8())));
				ExecCtx::get().ensureScript([targetPhysical]
				{
					targetPhysical->as<CommandList>()->recursivelyApplyDefaultState();
				});
				return;
			}
		}
		click.setResponse(LIT(LANG_FMT("CTX_UNAVAIL",
			FMT_ARG("option", LANG_GET("RGODFLT")),
			FMT_ARG("command", getTarget()->getMenuName().getLocalisedUtf8())
		)));
	}

	void ContextMenu::doMin(Click& click)
	{
		if (auto targetPhysical = getTargetPhysical())
		{
			if (targetPhysical->isT<CommandSlider>())
			{
				auto focus = targetPhysical->as<CommandSlider>();
				focus->setValue(click, focus->min_value);
				return;
			}
		}
		click.setResponse(LIT(LANG_FMT("CTX_UNAVAIL",
			FMT_ARG("option", LANG_GET("CTX_MIN")),
			FMT_ARG("command", getTarget()->getMenuName().getLocalisedUtf8())
		)));
	}

	void ContextMenu::doMax(Click& click)
	{
		if (auto targetPhysical = getTargetPhysical())
		{
			if (targetPhysical->isT<CommandSlider>())
			{
				auto focus = targetPhysical->as<CommandSlider>();
				focus->setValue(click, focus->max_value);
				return;
			}
		}
		click.setResponse(LIT(LANG_FMT("CTX_UNAVAIL",
			FMT_ARG("option", LANG_GET("CTX_MAX")),
			FMT_ARG("command", getTarget()->getMenuName().getLocalisedUtf8())
		)));
	}
}
