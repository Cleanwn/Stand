#pragma once

#include "CommandAction.hpp"

#include "FiberPool.hpp"
#include "LuaConfig.hpp"
#include "Tunables.hpp"

namespace Stand
{
	struct CommandLuaStart : public CommandAction
	{
		CommandLuaStart(CommandList* const parent, std::vector<CommandName>&& command_names)
			: CommandAction(parent, LOC("LUA_START"), std::move(command_names), NOLABEL, CMDFLAGS_ACTION | CMDFLAG_TEMPORARY)
		{
		}

		[[nodiscard]] CommandLuaScript* getScript() const noexcept
		{
			return parent->as<CommandLuaScript>();
		}

		void onClick(Click& click) final
		{
			if (g_tunables.getBool(TUNAHASH("DISABLE_LUAS")))
			{
				click.setResponse(LOC("CMDDISA"));
				return;
			}
			click.stopInputIfAllowed();
			if (!std::filesystem::is_regular_file(getScript()->getFilePath()))
			{
				click.setResponse(LOC("FILEGON"));
				return;
			}
			click.setNoResponse(); // Don't need generic response because of "Running {}..." toast
			auto code = getScript()->getCode();
			std::wstring warning{};
			if (LuaConfig::warn_dangerous->m_on && code.find("i_really_need_manual_access_to_process_apis") != std::string::npos)
			{
				warning = LANG_GET_W("LUA_W_OS");
			}
			else if (LuaConfig::warn_money->m_on && (
				code.find("MONEY.") != std::string::npos
				|| code.find("SERVICE_EARN_") != std::string::npos
				))
			{
				warning = LANG_GET_W("LUA_W_MNY");
			}
			if (warning.empty())
			{
				onClickPostWarning(click, std::move(code));
			}
			else
			{
				showWarning(click, std::move(warning), WARNING_COMMAND_BOUND, [wr{ getWeakRef() }, click, code{std::move(code)}](ThreadContext thread_context) mutable
				{
					if (auto self = wr.getPointer())
					{
						click.thread_context = thread_context;
						self->as<CommandLuaStart>()->onClickPostWarning(click, std::move(code));
					}
				});
			}
		}

		void onClickPostWarning(Click& click, std::string&& code)
		{
			const bool notify = !click.isAuto();
			FiberPool::queueJob([wr{ getWeakRef() }, notify, code{ std::move(code) }]() mutable
			{
				if (auto self = wr.getPointer())
				{
					self->as<CommandLuaStart>()->getScript()->run(notify, std::move(code));
				}
			});
		}
	};
}
