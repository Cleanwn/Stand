#include "RootMgr.hpp"

#include "CommandboxGrid.hpp"
#include "ContextMenu.hpp"
#include "Exceptional.hpp"
#include "FiberPool.hpp"
#include "get_root_list.hpp"
#include "Gui.hpp"
#include "MenuGrid.hpp"
#include "PlayerHistory.hpp"
#include "regular_event.hpp"
#include "Script.hpp"
#include "Worker.hpp"

namespace Stand
{
	static bool urs_applied_hotkeys;
	//static bool urs_applied_default;

	void RootMgr::updateRootState()
	{
		//SOUP_ASSERT(has_game_finished_loading());
		if (!g_gui.about_to_update_root_state)
		{
			g_gui.about_to_update_root_state = true;
			FiberPool::queueJob([]
			{
				updateRootStateImpl();
			});
		}
	}

	void RootMgr::updateRootStateImpl()
	{
		std::string focus_path_en;
		std::string web_focus_path_en;

		// Make it clear that root is updating now
		g_gui.about_to_update_root_state = true;
		if (g_gui.root_state != GUI_NONE)
		{
			g_menu_grid.updateNow();
		}
		// Await end of root check
		while (g_gui.root_check_in_progress)
		{
			Script::current()->yield();
			//soup::os::sleep(1);
		}
		// Synchronise with other threads
		EXCEPTIONAL_LOCK(Worker::root_update_mtx)
		EXCEPTIONAL_LOCK_WRITE(g_gui.root_mtx)
		g_gui.root_update_in_progress = true;
		// Save State
		if (g_gui.isRootStateFull())
		{
			if (g_gui.unlock_recover_state == 2)
			{
				g_gui.recover_state = std::unordered_map<std::string, std::string>();
				g_gui.saveStateInMemory(g_gui.recover_state.value());
			}
			for (const auto& list : g_gui.m_active_list)
			{
				auto tab = list->getTab();
				auto focus = list->getFocus();
				if (focus != nullptr)
				{
					auto hash = tab->menu_name.getHash();
					auto entry = g_gui.active_list_store.find(hash);
					if (entry == g_gui.active_list_store.end())
					{
						g_gui.active_list_store.emplace(hash, focus->getPathConfig(tab));
					}
					else
					{
						entry->second = focus->getPathConfig(tab);
					}
				}
			}
		}
		if (g_gui.root_state > GUI_MINIMAL)
		{
			const Command* const command = g_gui.getCurrentMenuFocus();
			if (command != nullptr)
			{
				focus_path_en = command->getPathConfig();
			}
		}
		if (g_gui.web_focus != nullptr)
		{
			web_focus_path_en = g_gui.web_focus->getPathConfig();
			g_gui.web_focus = nullptr;
		}
		// Reset state, soon-to-be-stale pointers, vectors, etc.
		/*urs_applied_default = false;
		FiberPool::queueJob([]
		{*/
			g_gui.root_list->recursivelyApplyDefaultState();
			/*urs_applied_default = true;
		});
		do
		{
			soup::os::sleep(50);
		} while (!urs_applied_default);*/
		g_gui.character_scale_affects_camera_command = nullptr;
		g_gui.character_scale_affects_speed_command = nullptr;
		g_gui.player_list = nullptr;
		for (auto& command : g_gui.player_commands)
		{
			command = nullptr;
		}
		g_gui.m_active_list.clear();
		g_gui.warnings.clear();
		CommandPhysical::collapse_command_names.clear();
		EXCEPTIONAL_LOCK(g_gui.commands_with_hotkeys_mtx)
		g_gui.commands_with_hotkeys.clear();
		EXCEPTIONAL_UNLOCK(g_gui.commands_with_hotkeys_mtx)
		g_gui.commands_with_correlation.clear();
		g_commandbox_grid.clearCache();
		// Detach player history command
		if (PlayerHistory::player_history_command->isAttached())
		{
			PlayerHistory::retained_player_history_command = PlayerHistory::player_history_command->detach();
		}
		// Some commands only register a tick handler and check their own state each tick to figure out if they should deactivate,
		// but because we are now replacing the root list in the same tick as we default states, we need them to unregister themselves before they go stale.
		reScriptTickEvent::trigger();
		// Update Root List
		ContextMenu::root.reset();
		updateRootStateCore();

		EXCEPTIONAL_UNLOCK_WRITE(g_gui.root_mtx)

		// Apply hotkeys off-thread
		urs_applied_hotkeys = false;
		auto apply_hotkeys_fn = []
		{
			EXCEPTIONAL_LOCK_READ(g_gui.root_mtx)
			g_gui.recursivelyApplyHotkeys(g_gui.root_list.get());
			EXCEPTIONAL_UNLOCK_READ(g_gui.root_mtx)
			urs_applied_hotkeys = true;
		};
		// Use worker if available to avoid having to spin up a new thread
		if (g_gui.worker_thread_running)
		{
			Worker::queueJob(apply_hotkeys_fn);
		}
		else
		{
			Exceptional::createManagedExceptionalThread(__FUNCTION__, apply_hotkeys_fn);
		}
		while (!urs_applied_hotkeys)
		{
			Script::current()->yield();
		}
		// Restore State
		for (auto i = g_gui.m_active_list.begin(); i != g_gui.m_active_list.end(); i++)
		{
			auto tab = (*i)->getTab();
			auto hash = tab->menu_name.getHash();
			auto entry = g_gui.active_list_store.find(hash);
			if (entry != g_gui.active_list_store.end())
			{
				CommandPhysical* focus = nullptr;
				EXCEPTIONAL_LOCK_READ(g_gui.root_mtx)
				focus = tab->resolveCommandConfig(entry->second);
				EXCEPTIONAL_UNLOCK_READ(g_gui.root_mtx)
				if (focus != nullptr)
				{
					*i = focus->parent;
					focus->focusInParent(TC_SCRIPT_YIELDABLE);
				}
			}
		}

		// Please note that this will cause CommandLuaScript::hold_up_save to be set too late, causing root update to erase lua command state.
		/*FiberPool::queueJob([]
		{*/
			if (g_gui.recover_state.has_value())
			{
				EXCEPTIONAL_LOCK_READ(g_gui.root_mtx)
				g_gui.recursivelyApplyState(g_gui.recover_state.value(), g_gui.root_list.get());
				EXCEPTIONAL_UNLOCK_READ(g_gui.root_mtx)
				if (g_gui.isRootStateFull())
				{
					g_gui.recover_state.reset();
				}
			}
			else
			{
				EXCEPTIONAL_LOCK_READ(g_gui.root_mtx)
				g_gui.recursivelyApplyState(g_gui.active_profile.data, g_gui.root_list.get());
				EXCEPTIONAL_UNLOCK_READ(g_gui.root_mtx)
				if (g_gui.isRootStateFull() && g_gui.unlock_recover_state == 1)
				{
					g_gui.unlock_recover_state = 2;
				}
			}
		//});

		// Doing this after state has loaded to allow detections state to be effective.
		if (g_gui.player_list != nullptr)
		{
			AbstractPlayer::updateLocalPlayer();
			if (PlayerHistory::inited)
			{
				g_gui.populatePlayerList(TC_SCRIPT_YIELDABLE);
			}
		}

		EXCEPTIONAL_UNLOCK(Worker::root_update_mtx)

		if (!focus_path_en.empty())
		{
			Command* const command = g_gui.root_list->resolveCommandEnglish(focus_path_en);
			if (command != nullptr)
			{
				command->focus(TC_SCRIPT_YIELDABLE, true);
			}
		}
		g_menu_grid.update();
		if (!web_focus_path_en.empty())
		{
			Command* const command = g_gui.root_list->resolveCommandEnglish(web_focus_path_en);
			if (command != nullptr && (command->type == COMMAND_LIST))
			{
				g_gui.web_focus = (CommandList*)command;
				g_gui.web_focus->updateWebState();
			}
			else
			{
				Exceptional::createManagedExceptionalThread(__FUNCTION__, []
				{
					g_gui.sendRootListToWeb();
				});
			}
		}

		g_gui.root_update_in_progress = false;
	}

	void RootMgr::updateRootStateCore()
	{
		g_gui.root_lang = Lang::active_id;
		g_gui.players_discovered = false;
		g_gui.pseudo_commands = std::make_unique<CommandList>(nullptr, NOLABEL);
		if (g_gui.unload_state == UnloadState::SWITCH_TO_GUI_NONE)
		{
			g_gui.root_state = GUI_NONE;
			g_gui.root_list = std::make_unique<CommandList>(nullptr, NOLABEL);
		}
		else
		{
			CommandPhysical::collapse_command_names.emplace(L"t");
			g_gui.root_list = std::unique_ptr<CommandList>(get_root_list());
		}
		g_gui.about_to_update_root_state = false;
		g_gui.root_cursor = ((g_gui.root_state == GUI_FREE) ? 1 : 0);
		g_gui.updateTabs(TC_SCRIPT_YIELDABLE);
	}
}
