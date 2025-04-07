#include "tbDeceiveInteriorChecks.hpp"

#include "is_session.hpp"
#include "NativeTableHooks.hpp"
#include "ScriptGlobal.hpp"
#include "scrNativeCallContext.hpp"

namespace Stand
{
	void tbDeceiveInteriorChecks::onEnable()
	{
		tbToggleableBehaviourScript::onEnable();
		NativeTableHooks::createNativeEntrypointOverride(0x2107BA504071A6BB, [](rage::scrNativeCallContext& ctx)
		{
			NativeTableHooks::og(0x2107BA504071A6BB)(ctx);
			if (is_session_started()) // Can break story mode missions, e.g. Complications.
			{
				switch (ctx.getReturnValue<int>())
				{
				case 3330: // Lifeinvader
				case 178433: // Omega's Garage
				case 184321: // Humane Labs
				case 197377: // Hayes Autos
				case 233729: // Tequi-La-La
				case 245761: // Lost MC Clubhouse
				case 250625: // Foundry
					ctx.setReturnValue<int>(0);
				}
			}
		});
	}

	void tbDeceiveInteriorChecks::onDisable()
	{
		NativeTableHooks::removeNativeEntrypointOverride(0x2107BA504071A6BB);
	}

	void tbDeceiveInteriorChecks::onTick()
	{
		// Union Depository, FIB
		//ScriptGlobal(g_SimpleInteriorData_eKilledScriptForInteriorID).set<int>(0);

		// Union Depository, FIB, Clucking Bell Factory
		ScriptGlobal(g_MPRestrictedInteriorAccessData_iKickOutFromRestrictedInteriorBS).at(0, 1).set<>(0);
	}
}
