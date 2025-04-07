#include "CommandLuaAction.hpp"

#include "StringUtils.hpp"

namespace Stand
{
	CommandLuaAction::CommandLuaAction(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, CommandLuaScript* const script, const int on_click_ref, const int on_command_ref, std::wstring&& syntax, CommandPerm perm)
		: CommandLuaPhysical(parent, std::move(menu_name), std::move(command_names), std::move(help_text), script, 0, perm), on_click_ref(on_click_ref), on_command_ref(on_command_ref), syntax(std::move(syntax))
	{
	}

	std::wstring CommandLuaAction::getCommandSyntax() const
	{
		return syntax.empty() ? CommandLuaPhysical::getCommandSyntax() : syntax;
	}

	void CommandLuaAction::onClick(Click& click)
	{
		const auto click_type = click.type;
		const auto issued_for = click.issued_for;
		lua_data.queueJob(this, click.type, [this, click_type, issued_for](lua_State* L, bool direct)
		{
			luaS_invoke_void_variable(L, direct, on_click_ref, click_type, issued_for);
		});
	}

	void CommandLuaAction::onCommand(Click& click, std::wstring& args)
	{
		if (on_command_ref == LUA_NOREF)
		{
			CommandPhysical::onCommand(click, args);
		}
		else
		{
			// Don't show "Command executed successfully", let script handle toasts...
			click.setNoResponse();

			auto args_utf8 = StringUtils::utf16_to_utf8(args);
			args.clear();
			const auto click_type = click.type;
			const auto issued_for = click.issued_for;
			lua_data.queueJob(this, click.type, [this, args_utf8{ std::move(args_utf8) }, click_type, issued_for](lua_State* L, bool direct)
			{
				luaS_invoke_void_variable(L, direct, on_command_ref, args_utf8, click_type, issued_for);
			});
		}
	}
}
