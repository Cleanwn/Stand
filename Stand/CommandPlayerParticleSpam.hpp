#pragma once

#include "CommandPlayerToggle.hpp"

#include "CommandPlayerParticleSpamDelay.hpp"
#include "CommandPlayerParticleSpamScale.hpp"
#include "CommandSliderFloat.hpp"
#include "Particles.hpp"
#include "natives.hpp"

namespace Stand
{
	class CommandPlayerParticleSpam : public CommandPlayerToggle
	{
	private:
		time_t last_fx;
		CommandListSelect* const particles;
		CommandPlayerParticleSpamDelay* const delay;
		CommandPlayerParticleSpamScale* const scale;
		CommandToggle* const hide_locally;

	public:
		explicit CommandPlayerParticleSpam(CommandList* const parent, CommandListSelect* const particles, CommandPlayerParticleSpamDelay* const delay, CommandPlayerParticleSpamScale* const scale, CommandToggle* const hide_locally)
			: CommandPlayerToggle(parent, LOC("PLRPTFXSPM"), CMDNAMES_OBF("particlespam")), delay(delay), particles(particles), scale(scale), hide_locally(hide_locally)
		{
		}

		void onEnable(Click& click)
		{
			registerScriptTickEventHandler(click, [this]
			{
				HANDLER_CHECK(this->m_on);
				if (auto ped = AbstractPlayer(this->getPlayer()).getPed(); ped.isValid())
				{
					const auto delay = this->delay->value;
					if (!delay
						|| GET_MILLIS_SINCE(last_fx) >= delay
						)
					{
						if (hide_locally->m_on)
						{
							const auto particle = (const Particle*)this->particles->value;
							Particles::request(particle);
							Particles::use(particle);

							// pointers::rage_ptfxScript_GetTriggeredAlphaTint(&a);
							// pointers::rage_ptfxScript_GetTriggeredColourTint(&r, &g, &b);

							unsigned int ptfxhash = rage::atStringHash(particle->rawName);
							unsigned int ptfxassethash = rage::atStringHash(particle->assetCategory);

							auto pos = rage::Vector3(ped.getPos());
							auto rot = rage::Vector3(0.0f, 180.0f, 0.0f);
							auto scale = this->scale->getFloatValue();
							auto invertAxes = false;
							auto r = -1.0f;
							auto g = -1.0f;
							auto b = -1.0f;
							auto a = 1.0f;

							pointers::CNetworkPtFXEvent_Trigger(
								&ptfxhash,
								&ptfxassethash,
								nullptr,
								-1,
								&pos,
								&rot,
								&scale,
								&invertAxes,
								&r,
								&g,
								&b,
								&a,
								false
							);
						}
						else
						{
							Particles::tryPlay(this->particles->value, ped.getPos(), this->scale->getFloatValue());
						}
						last_fx = get_current_time_millis();
					}
				}
				HANDLER_END;
			});
		}
	}; 
}