#include "CommandInputRid.hpp"

#include <fmt/format.h>

#include "get_next_arg.hpp"
#include "lang.hpp"
#include "str2int.hpp"

namespace Stand
{
    std::wstring CommandInputRid::getCommandSyntax() const
    {
		return std::move(LANG_GET_W("CMD").append(L": ").append(cmdNameToUtf16(command_names.at(0))).append(L" [rid]"));
    }

    void CommandInputRid::onCommand(Click& click, std::wstring& args)
    {
		auto arg = get_next_arg(args);
		if (arg.empty())
		{
			return onClick(click);
		}
		try
		{
			int64_t rid = std::stoll(arg);
			onCommand(click, rid);
			setValue(fmt::to_string(rid), click.type);
		}
		catch (const std::exception&)
		{
			onCommandInvalidArg(click, arg);
		}
    }
	
	int64_t CommandInputRid::getRid() const noexcept
	{
		return str2int<int64_t>(value, 0);
	}
}
