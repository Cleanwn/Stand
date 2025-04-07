#pragma once

#include "CommandToggle.hpp"
#include "Hooking.hpp"
#include "lang.hpp"

namespace Stand
{
	class CommandBlockJoinsNonWhitelisted : public CommandToggle
	{
	public:
		explicit CommandBlockJoinsNonWhitelisted(CommandList* const parent)
			: CommandToggle(parent, LOC("BJ_WHT"), {}, LOC("BJ_WHT_H"))
		{
		}

		Label getActivationName() const final
		{
			return LOC("BJ_WHT_A");
		}

		void onChange(Click& click) final
		{
			g_hooking.block_joins_from_non_whitelisted = m_on;
		}
	};
}
