#pragma once

#include "CommandList.hpp"

namespace Stand
{
	class CommandListAnimations : public CommandList
	{
	public:
		static inline bool looped = true;
		static inline bool partial_movement = false;

		explicit CommandListAnimations(CommandList* const parent);
	};
}