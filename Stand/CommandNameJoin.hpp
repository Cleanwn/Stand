#pragma once

#include "CommandTextsliderJoin.hpp"

#include "get_next_arg.hpp"
#include "JoinUtil.hpp"
#include "ScAccount.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	class CommandNameJoin : public CommandTextsliderJoin
	{
	public:
		explicit CommandNameJoin(CommandList* const parent)
			: CommandTextsliderJoin(parent, LOC("JOIN_N"), CMDNAMES_OBF("join", "namejoin"))
		{
		}

		std::wstring getCommandSyntax() const final
		{
			return std::move(LANG_GET_W("CMD").append(L": ").append(cmdNameToUtf16(command_names.at(0))).append(L" ").append(LANG_GET_W("ARGNME")));
		}

		void onClick(Click& click) final
		{
			if (click.isBasicEdition())
			{
				CommandTextsliderJoin::onClick(click);
			}
		}

		void onCommand(Click& click, std::wstring& args) final
		{
			if (click.isBasicEdition() && ScAccount::name2rid(click, StringUtils::utf16_to_utf8(get_next_arg(args)), [this](const ScAccount& a)
			{
				JoinUtil::join(a.rid, value);
			}))
			{
				return onClick(click);
			}
		}
	};
}
