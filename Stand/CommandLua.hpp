#pragma once

#include <vector>

#include <soup/WeakRef.hpp>

#include "CommandListPlayer.hpp"
#include "CommandLuaScript.hpp"
#include "Util.hpp"

namespace Stand
{
	struct CommandLuaData
	{
		CommandLuaScript* const script;
		std::string name_for_config{};
		std::vector<int> on_tick_in_viewport_handlers{};
		std::vector<int> on_focus_handlers{};
		std::vector<int> on_blur_handlers{};

		void queueJob(Command* cmd, ClickType click_type, std::function<void(lua_State*, bool)>&& func);
	};

	template <class T>
	class CommandLua : public T
	{
	public:
		CommandLuaData lua_data;

		template <typename...Args>
		CommandLua(CommandList* const parent, CommandLuaScript* const script, Args&&...args)
			: T(parent, std::forward<Args>(args)...), lua_data({ script })
		{
			T::has_lua_data = true;
		}

		template <typename...Args>
		CommandLua(CommandList* const parent, Label&& menu_name, CommandLuaScript* const script, Args&&...args)
			: T(parent, std::move(menu_name), std::forward<Args>(args)...), lua_data({ script })
		{
			T::has_lua_data = true;
		}

		template <typename...Args>
		CommandLua(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, CommandLuaScript* const script, Args&&...args)
			: T(parent, std::move(menu_name), std::move(command_names), std::move(help_text), std::forward<Args>(args)...), lua_data({ script })
		{
			T::has_lua_data = true;
		}
	};

	template <class T>
	class CommandLuaPhysical : public CommandLua<T>
	{
	private:
		using Base = CommandLua<T>;

		bool focused = false;

	public:
		using Base::Base;

		void preDelete() final
		{
			T::preDelete();

			if (Base::lua_data.script->running)
			{
				luaS_releaseReferences(Base::lua_data.script->L, Base::lua_data.on_tick_in_viewport_handlers);
				luaS_releaseReferences(Base::lua_data.script->L, Base::lua_data.on_focus_handlers);
				luaS_releaseReferences(Base::lua_data.script->L, Base::lua_data.on_blur_handlers);
			}
		}

		[[nodiscard]] Label getActivationName() const final
		{
			if (auto pc = Base::resolveParent(COMMAND_LIST_PLAYER)->as<CommandListPlayer>())
			{
				return LIT(LANG_FMT("PLY_A_LUA", fmt::arg("script", Base::lua_data.script->menu_name.getLocalisedUtf8()), fmt::arg("command", Base::menu_name.getLocalisedUtf8()), fmt::arg("player", pc->getPlayerName())));
			}
			return Base::getActivationName();
		}

		void onTickInGameViewport() final
		{
			T::onTickInGameViewport();

			if (!Base::lua_data.script->off_thread)
			{
				luaS_invoke_void(Base::lua_data.script->L, Base::lua_data.on_tick_in_viewport_handlers);
			}
		}

		void onTickInWebViewport() final
		{
			return T::onTickInWebViewportImplRedirect();
		}

		[[nodiscard]] std::string getNameForConfig() const final
		{
			if (!Base::lua_data.name_for_config.empty())
			{
				return Base::lua_data.name_for_config;
			}
			return T::getNameForConfig();
		}

		void onFocus(ThreadContext thread_context, Direction momentum) final
		{
			T::onFocus(thread_context, momentum);

			if (!focused) // Workaroud for onFocus/onBlur not having once-off guarantee which Lua dev might expect
			{
				focused = true;
				if (!Base::lua_data.on_focus_handlers.empty())
				{
					auto wr = T::getWeakRef();
					Util::ensureScriptThreadIfAllowed(thread_context, [this, momentum, wr{ std::move(wr) }]
					{
						if (wr.isValid())
						{
							luaS_invoke_void(Base::lua_data.script->L, Base::lua_data.on_focus_handlers, momentum);
						}
					});
				}
			}
		}

		void onBlur(ThreadContext thread_context, Direction momentum) final
		{
			T::onBlur(thread_context, momentum);

			if (focused)
			{
				focused = false;
				if (!Base::lua_data.on_blur_handlers.empty())
				{
					auto wr = T::getWeakRef();
					Util::ensureScriptThreadIfAllowed(thread_context, [this, momentum, wr{ std::move(wr) }]
					{
						if (wr.isValid())
						{
							luaS_invoke_void(Base::lua_data.script->L, Base::lua_data.on_blur_handlers, momentum);
						}
					});
				}
			}
		}
	};
}
