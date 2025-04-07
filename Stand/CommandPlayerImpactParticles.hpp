#pragma once

#include "CommandListSelectParticle.hpp"
#include "CommandOnPlayer.hpp"

namespace Stand
{
	class CommandPlayerImpactParticles : public CommandOnPlayer<CommandListSelectParticle>
	{
	public:
		explicit CommandPlayerImpactParticles(CommandList* const parent)
			: CommandOnPlayer(parent, LOC("IMPPART"), LOC("PLYEXPAMMO_H"), 0, 0)
		{
			auto options = this->getOptions();
			options.insert(options.begin(), CommandListActionItemData(0, LOC("DOFF")));
			this->setOptions(options);
		}

		void onChange(Click& click, long long prev_value) final
		{
			const auto value = this->value;
			if (value != 0)
			{
				registerScriptTickEventHandler([this, value]
				{
					HANDLER_CHECK(this->value == value);
					for (const auto& p : PP_PTR->getPlayers())
					{
						if (auto ped = p.getPed(); ped.isValid())
						{
							Vector3 coords;

							if (WEAPON::GET_PED_LAST_WEAPON_IMPACT_COORD(ped, &coords))
							{
								Particles::tryPlay(value, std::move(coords));
							}
						}
					}
					HANDLER_END;
				});
			}
		}
	};
}