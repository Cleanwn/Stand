#pragma once

#include "CommandPlayerToggle.hpp"

namespace Stand
{
	class CommandPlayerFakeMoneyDrop : public CommandPlayerToggle
	{
	private:
		CGameScriptId dummy_script{};
		std::vector<AbstractEntity> bags{};

		[[nodiscard]] v3 makePos(v3&& pos)
		{
			pos.x += (float)soup::rand(-2, 2) / 10.0f;
			pos.y += (float)soup::rand(-2, 2) / 10.0f;
			pos.z += 1.25f;

			return pos;
		}

	public:
		explicit CommandPlayerFakeMoneyDrop(CommandList* const parent)
			: CommandPlayerToggle(parent, LOC("FAKEDROP"), CMDNAMES_OBF("fakemoneydrop"))
		{
			// Judging by CProjectile::ProcessProximityMineTrigger, these values can afford to be worthless.
			dummy_script.m_unk = 0;
			dummy_script.m_hash = 0;
			dummy_script.m_name[0] = 0;
			dummy_script.m_timestamp = 0;
			dummy_script.instance_id = -1;
			dummy_script.m_position_hash = 0;
		}

		void onEnable(Click& click) final
		{
			registerScriptTickEventHandler([this]
			{
				HANDLER_CHECK(this->m_on);

				if (STREAMING::HAS_MODEL_LOADED(ATSTRINGHASH("p_poly_bag_01_s")))
				{
					for (const auto& p : PP_PTR->getPlayers())
					{
						auto pos = makePos(p.getPos());
						auto bag = Util::createObject(ATSTRINGHASH("p_poly_bag_01_s"), pos);
						ENTITY::APPLY_FORCE_TO_ENTITY(bag, 3, 0.0f, 0.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0, true, false, false, false, false);

						if (p == g_player)
						{
							AUDIO::PLAY_SOUND_FROM_COORD(-1, "LOCAL_PLYR_CASH_COUNTER_COMPLETE", pos.x, pos.y, pos.z, "DLC_HEISTS_GENERAL_FRONTEND_SOUNDS", FALSE, 2, FALSE);
						}
						else // Let's not troll ourselves either.
						{
							pointers::CPlaySoundEvent_TriggerPos(&pos, ATSTRINGHASH("DLC_HEISTS_GENERAL_FRONTEND_SOUNDS"), ATSTRINGHASH("LOCAL_PLYR_CASH_COUNTER_COMPLETE"), -1, &dummy_script, 2);
						}

						bag.setCanMigrate(false);
						bags.emplace_back(bag);
					}

					if (bags.size() > 150)
					{
						bags.front().removeFromPlaneOfExistence();
						bags.erase(bags.begin());
					}
				}
				else
				{
					STREAMING::REQUEST_MODEL(ATSTRINGHASH("p_poly_bag_01_s"));
				}

				HANDLER_END;
			});
		}

		void onDisable(Click& click) final
		{
			for (auto& bag : bags)
			{
				bag.removeFromPlaneOfExistence();
			}

			bags.clear();
		}
	};
}