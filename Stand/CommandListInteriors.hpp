#pragma once

#include "CommandList.hpp"

#include "CommandInterior.hpp"

namespace Stand
{
	class CommandListInteriors : public CommandList
	{
	public:
		inline static CommandInterior* lscm;
		inline static CommandInterior* server_room;
		inline static CommandInterior* ranch;
		inline static CommandInterior* bahamamamas;
		inline static CommandInterior* therapy;
		inline static CommandInterior* sol;
		inline static CommandInterior* benny;
		inline static CommandInterior* chopshop;
		inline static CommandInterior* tequilala;
		inline static CommandInterior* udg;
		inline static CommandInterior* eps;

		explicit CommandListInteriors(CommandList* parent);
	};
}
