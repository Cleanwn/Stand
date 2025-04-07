#pragma once

#include "CommandToggle.hpp"

#include <fmt/xchar.h>

#include <soup/rand.hpp>

#include "DrawUtil3d.hpp"
#include "get_ground_z.hpp"
#include "lang.hpp"
#include "natives.hpp"
#include "PosRot.hpp"
#include "regular_event.hpp"
#include "Renderer.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandGeoGuessrMain : public CommandToggle
	{
	public:
		PosRot target{};

		inline static PosRot possible_targets[] = {
			// Expert
			{ { 1201.0f, -3112.0f, 6.3f }, { 0.0f, 0.0f, -134.0f } },
			{ { -183.9f, 51.8f, 71.0f }, { -1.5f, 0.0f, -105.0f } },
			{ { -1564.0f, -417.1f, 39.6f }, { -10.0f, 0.0f, -69.0f } },
			{ { 225.6f, -102.2f, 87.9f }, { -90.0f, 0.0f, 69.0f } },
			{ { -227.0f, 155.4f, 75.0f }, { 2.8f, 0.0f, 75.0f } },
			{ { -1221.2f, -1301.28f, 12.3f }, { -3.7f, 0.0f, 63.4f } },
			{ { -17.0f, 6465.0f, 32.4f }, { -2.8f, 0.0f, 57.4f } },
			{ { -350.9f, 54.8f, 55.5f }, { -10.4f, 0.0f, -118.3f } },
			{ { -1548.8f, 132.6f, 59.2f }, { 0.0f, 0.0f, 166.7f } },
			{ { -609.8f, -780.f, 26.4f }, { -1.5f, 0.0f, -88.3f } },
			{ { -1086.5f, -365.4f, 39.0f }, { 1.4f, 0.0f, 42.8f } },
			{ { -1449.7f, 549.0f, 122.0f }, { -8.4f, 0.0f, -16.6f } },
			{ { 846.0f, -2501.0f, 41.4f }, { -4.5f, 0.0f, -110.4f } },
			{ { -1121.0f, -2818.f, 40.8f }, { -4.6f, 0.0f, 81.3f } },

			// Hardcore
			{ { 867.5963f, -2317.6f, 31.2f }, { 0.0f, 0.0f, -17.7f } },
			{ { 1903.8f, 3709.0f, 33.7f }, { -16.0f, 0.0f, 28.0f } },
		};

		explicit CommandGeoGuessrMain(CommandList* const parent)
			: CommandToggle(parent, LOC("GGSR"), {}, NOLABEL, false, CMDFLAGS_TOGGLE & ~CMDFLAG_SUPPORTS_STATE_OPERATIONS)
		{
		}

		void onEnable(Click& click) final
		{
			ensureYieldableScriptThread(click, [this]
			{
				newRound();
				parent->as<CommandGeoGuessr>()->updateGameCommandsVisiblity(true);
				registerScriptTickEventHandlerInContext([this]
				{
					if (!m_on)
					{
						return false;
					}
					if (parent->as<CommandGeoGuessr>()->scouting)
					{
						if (Camgr::canCamCommandRun())
						{
							Camgr::lock_focus(target.pos);
							Camgr::lock_pos(target.pos);
							Camgr::lock_rot(target.rot);
							Camgr::lock_fov(50.0f);
							PAD::DISABLE_ALL_CONTROL_ACTIONS(2);
						}
						else
						{
							parent->as<CommandGeoGuessr>()->stopScouting();
						}
					}
					else if (parent->as<CommandGeoGuessr>()->guessed_at != 0)
					{
						if (!Camgr::canCamCommandRun())
						{
							setOnIndication(false);
							parent->as<CommandGeoGuessr>()->cleanup();
							return false;
						}
						if (GET_MILLIS_SINCE(parent->as<CommandGeoGuessr>()->guessed_at) > 2000)
						{
							parent->as<CommandGeoGuessr>()->guessed_at = 0;
							parent->as<CommandGeoGuessr>()->cleanupGuess();
							newRound();
						}
						else
						{
							auto vec = parent->as<CommandGeoGuessr>()->guess.lookAt(target.pos).toDirNoZ();
							auto dist = parent->as<CommandGeoGuessr>()->guess.distance(target.pos) * 0.5f;
							vec *= dist;
							vec += parent->as<CommandGeoGuessr>()->guess;
							vec.z += dist;
							Camgr::lock_pos(vec);
							Camgr::lock_focus(vec);
							Camgr::lock_rot({ -90.0f, 0.0f, 0.0f });
							Camgr::lock_fov(100.0f);
							DrawUtil3d::exclusive_ar = true;
							DrawUtil3d::draw_line_directx(parent->as<CommandGeoGuessr>()->guess, target.pos, g_renderer.arColour);
							DrawUtil3d::draw_ar_marker(target.pos);
							auto score_dist = Util::VDIST(parent->as<CommandGeoGuessr>()->guess.x, parent->as<CommandGeoGuessr>()->guess.y, target.pos.x, target.pos.y);
							if (score_dist < 10.0f)
							{
								g_renderer.drawCentredTextThisTick(LANG_GET_W("GGSR_WIN"));
							}
							else
							{
								g_renderer.drawCentredTextThisTick(fmt::format(fmt::runtime(LANG_GET_W("GGSR_OFF")), (int)score_dist));
							}
						}
					}
					return true;
				});
			});
		}

		void onDisable(Click& click) final
		{
			ensureScriptThread(click, [this]
			{
				parent->as<CommandGeoGuessr>()->cleanup();
			});
		}

	private:
		void newRound()
		{
			PosRot next_target;
			do
			{
				next_target = ARRAY_RAND(possible_targets);
			} while (next_target.pos == target.pos);
			target = std::move(next_target);
			parent->as<CommandGeoGuessr>()->startScouting();
			parent->as<CommandGeoGuessr>()->scout_toggle->focusInParent(TC_SCRIPT_YIELDABLE);
		}
	};
}
