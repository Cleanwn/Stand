#include "CommandPlayerExplode.hpp"

#include "AbstractEntity.hpp"
#include "eExplosionTag.hpp"
#include "natives.hpp"
#include "PlayerProvider.hpp"
#include "script_thread.hpp"
#include "Vector2Plus.hpp"

namespace Stand
{
	CommandPlayerExplode::CommandPlayerExplode(CommandList* parent)
		: CommandPlayerActionAggressive(parent, LOC("PLYEXP"), CMDNAMES_OBF("explode"))
	{
	}

	void CommandPlayerExplode::onClick(Click& click)
	{
		ensureScriptThread(click, [this, click]
		{
			if (click.issuer_is_explicit)
			{
				for (const auto& p : PP_PTR->getPlayers())
				{
					auto pos = p.getPos();
					FIRE::ADD_OWNED_EXPLOSION(click.getEffectiveIssuer().getPed(), pos.x, pos.y, pos.z, EXP_TAG_STICKYBOMB, 1.0f, true, false, 1.0f);
				}
			}
			else
			{
				for (const auto& p : PP_PTR->getPlayers())
				{
					auto pos = p.getPos();
					FIRE::ADD_EXPLOSION(pos.x, pos.y, pos.z, EXP_TAG_STICKYBOMB, 1.0f, true, false, 1.0f, false);
				}
			}
		});
	}
}
