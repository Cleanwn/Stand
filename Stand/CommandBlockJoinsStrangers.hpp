#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandBlockJoinsStrangers : public CommandToggle
	{
	public:
		explicit CommandBlockJoinsStrangers(CommandList* const parent)
			: CommandToggle(parent, LOC("BJ_STR"), CMDNAMES_OBF("blockjoinsstrangers", "blockjoinstrangers", "blockjoinsfromstrangers"), LOC("BJ_STR_H"))
		{
		}

		Label getActivationName() const final
		{
			return LOC("BJ_STR_A");
		}

		void onChange(Click& click) final
		{
			g_hooking.block_joins_from_strangers = m_on;
		}
	};
}
