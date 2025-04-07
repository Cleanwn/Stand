#pragma once

#include "CommandListSelectParticle.hpp"

#include "FiberPool.hpp"

namespace Stand
{
	class CommandTeleportParticle : public CommandListSelectParticle
	{
	public:
		inline static CommandListSelect* instance = nullptr;

		explicit CommandTeleportParticle(CommandList* const parent)
			: CommandListSelectParticle(parent, LOC("TPFX"), NOLABEL, INCLUDE_NONE, 0)
		{
			this->instance = this;
		}

		static void playTpParticle(const v3& pos)
		{
			FiberPool::queueJob([pos]
			{
				if (const auto value = CommandTeleportParticle::instance->value)
				{
					Particles::play(value, pos);
				}
			});
		}
	};
}