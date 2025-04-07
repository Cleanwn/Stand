#include "CommandPlayerToggleBatch.hpp"

#include "AbstractEntity.hpp"
#include "AbstractPlayer.hpp"
#include "explosion.hpp"
#include "gta_ped.hpp"

namespace Stand
{
	class CommandPlayerShakeCam : public CommandPlayerToggleBatch
	{
	public:
		explicit CommandPlayerShakeCam(CommandList* const parent)
			: CommandPlayerToggleBatch(parent, LOC("PLYSHAKECAM"), { CMDNAME("shakecam"), CMDNAME("shakecamera") }, NOLABEL, COMMANDPERM_RUDE)
		{
		}

		void onTick(const std::vector<AbstractPlayer>& players) const final
		{
			for (const AbstractPlayer& p : players)
			{
				const auto pos = p.getPos();
				if (p == g_player)
				{
					FIRE::ADD_OWNED_EXPLOSION(g_player_ped, pos.x, pos.y, pos.z, EXP_TAG_GRENADE, 0.0f, false, true, 1.0f);
				}
				else
				{
					CExplosionManager::CExplosionArgs args{ EXP_TAG_GRENADE, pos };
					args.m_pEntExplosionOwner = (CEntity*)g_player_ped.getCPed();
					args.m_bMakeSound = false;
					args.m_fCamShake = 1.0f;
					args.m_bNoDamage = true;
					args.m_sizeScale = 0.0f;
					args.m_bNoFx = true;
					pointers::CExplosionEvent_Trigger(&args, nullptr);
				}
			}
		}
	};
}
