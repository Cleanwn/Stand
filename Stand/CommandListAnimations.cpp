#include "CommandListAnimations.hpp"

#include "Animations.hpp"
#include "CommandAnimation.hpp"

namespace Stand
{
	CommandListAnimations::CommandListAnimations(CommandList* const parent)
		: CommandList(parent, LOC("ANIM"), CMDNAMES("animations"))
	{
		// Dances
		{
			auto dances = this->createChild<CommandList>(LOC("ANIM_DANCE"), CMDNAMES("dances"));

			for (auto& dance : Animations::dances)
			{
				dances->createChild<CommandAnimation<ComplexAnimation>>(dance);
			}
		}

		for (auto& anim : Animations::animations)
		{
			this->createChild<CommandAnimation<ComplexAnimation>>(anim);
		}
	}
}
