#pragma once

#include "CommandPlayerCrash.hpp"

#include "natives.hpp"
#include "using_model.hpp"

namespace Stand
{
	class CommandPlayerCrashFootlettuce : public CommandPlayerCrash
	{
	public:
		explicit CommandPlayerCrashFootlettuce(CommandList* const parent)
			: CommandPlayerCrash(parent, LOC("PLYCRSH_G"), CMDNAMES_OBF("footlettuce", "lettuce"), LOC("PLYKCK_H_B_P"))
		{
		}

		void expandDong(std::vector<AbstractPlayer>&& targets) const final
		{
			auto p = targets.at(0);
			if (!p.blockingOutgoingSyncs()
				&& !p.isRockstarAdmin()
				)
			{
				STREAMING::REQUEST_MODEL(Hooking::footlettuce_model_to);
				using_model_auto(Hooking::footlettuce_model_from, [p]
				{
					//Util::toast("model 1 loaded", TC_SCRIPT_YIELDABLE);
					while (!STREAMING::HAS_MODEL_LOADED(Hooking::footlettuce_model_to))
					{
						Script::current()->yield();
					}
					//Util::toast("model 2 loaded", TC_SCRIPT_YIELDABLE);
					g_hooking.footlettuce_target = p.getCNetGamePlayer();
					auto obj = Util::createObject(Hooking::footlettuce_model_from, p.getPos());
					while (g_hooking.footlettuce_target != nullptr)
					{
						Script::current()->yield();
					}
					obj.removeFromPlaneOfExistence();
				});
			}
		}
	};
}
