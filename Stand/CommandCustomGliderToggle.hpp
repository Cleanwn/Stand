#pragma once

#include "CommandToggle.hpp"

#include "AbstractEntity.hpp"
//#include "drawDebugText.hpp"
#include "gta_input.hpp"
#include "gta_vehicle.hpp"
#include "input.hpp"
#include "natives.hpp"

namespace Stand
{
#pragma pack(push, 1)
	class CommandCustomGliderToggle : public CommandToggle
	{
	private:
		bool extending = false;
		float off = -0.15f;

	public:
		explicit CommandCustomGliderToggle(CommandList* const parent)
			: CommandToggle(parent, LOC("CSTWNGBVHR"), CMDNAMES("wingsmod"), LOC("CSTWNGBVHR_H"))
		{
		}

		void onEnable(Click& click) final
		{
			if (auto v = g_player_veh.getCVehicle())
			{
				if (v->m_gliderState == CVehicle::DEPLOYING_WINGS || v->m_gliderState == CVehicle::GLIDING)
				{
					extending = true;
					off = v->m_wingDeployedRatio;
				}
				else
				{
					extending = false;
					off = ((float(parent->as<CommandCustomGlider>()->min->value) / 100.0f) - 0.15f);
				}
			}

			registerScriptTickEventHandler(click, [this]
			{
				if (!m_on)
				{
					return false;
				}
				auto v = g_player_veh.getCVehicle();
				if (v)
				{
					//drawDebugText((int)v->m_gliderState);
					//drawDebugText(v->m_wingDeployedRatio);

					if (Input::isControlJustPressed(
						g_player_veh.getModelHash() == ATSTRINGHASH("oppressor2")
						? /* [E] */ INPUT_VEH_ROCKET_BOOST
						: /* [X] */ INPUT_VEH_BIKE_WINGS
					))
					{
						extending = !extending;
					}

					if (extending)
					{
						const float max = ((float(parent->as<CommandCustomGlider>()->max->value) / 100.0f) - 0.15f);
						if (off == max)
						{
							v->m_gliderState = CVehicle::GLIDING;
							v->m_wingDeployedRatio = max;
						}
						else
						{
							off += (float(parent->as<CommandCustomGlider>()->speed->value) / 1000.0f);
							if (off > max)
							{
								off = max;
							}
							v->m_gliderState = CVehicle::DEPLOYING_WINGS;
							v->m_wingDeployedRatio = off;
						}
					}
					else
					{
						const float min = ((float(parent->as<CommandCustomGlider>()->min->value) / 100.0f) - 0.15f);
						if (off == min)
						{
							v->m_gliderState = CVehicle::NOT_GLIDING;
							v->m_wingDeployedRatio = min;
						}
						else
						{
							off -= (float(parent->as<CommandCustomGlider>()->speed->value) / 1000.0f);
							if (off <= min)
							{
								off = min;
							}
							v->m_gliderState = CVehicle::DEPLOYING_WINGS;
							v->m_wingDeployedRatio = off;
						}
					}
				}
				else
				{
					extending = false;
					off = ((float(parent->as<CommandCustomGlider>()->min->value) / 100.0f) - 0.15f);
				}
				return true;
			});
		}
	};
#pragma pack(pop)
}
