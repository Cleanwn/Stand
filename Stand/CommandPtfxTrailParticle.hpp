#pragma once

#include "CommandListSelectParticle.hpp"

#include "CommandPtfxTrail.hpp"
#include "CommandSliderFloat.hpp"

namespace Stand
{
	class CommandPtfxTrailParticle : public CommandListSelectParticle
	{
	public:
		CommandSlider* interval;
		CommandSliderFloat* scale;
		CommandSliderFloat* offset;

		explicit CommandPtfxTrailParticle(CommandList* const parent)
			: CommandListSelectParticle(parent, LOC("PARTICLE"), NOLABEL, INCLUDE_NONE, 0)
		{
		}

		void onChange(Click& click, long long prev_value) final
		{
			if (const auto val = this->value)
			{
				// Using TC_OTHER to force a yield so when state is loaded, we will get the bones right on the first tick.
				registerScriptTickEventHandler(TC_OTHER, [this, val]
				{
					HANDLER_CHECK(this->value == val);

					this->parent->forEachPhysicalChild([this](CommandPhysical* cmd)
					{
						if (cmd->isToggle())
						{
							if (auto p = cmd->as<CommandPtfxTrail>(); p->m_on)
							{
								if (interval->value == 0 || GET_MILLIS_SINCE(p->last_applied) >= interval->value)
								{
									const auto particle = reinterpret_cast<const Particle*>(value);
									Particles::request(particle);
									if (Particles::hasLoaded(particle))
									{
										Particles::use(particle);
										GRAPHICS::START_NETWORKED_PARTICLE_FX_NON_LOOPED_ON_PED_BONE(particle->rawName, g_player_ped, 0.0f, 0.0f, offset->getFloatValue(), 0.0f, 180.0f, 0.0f, p->bone, scale->getFloatValue(), FALSE, FALSE, FALSE);
										STREAMING::REMOVE_NAMED_PTFX_ASSET(particle->rawName);
										p->last_applied = get_current_time_millis();
									}
								}
							}
						}
					});

					HANDLER_END;
				});
			}
		}
	};
}