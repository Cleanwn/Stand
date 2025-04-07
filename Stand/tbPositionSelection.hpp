#pragma once

#include "tbToggleableBehaviourScript.hpp"

#include "natives_decl.hpp"
#include "vector.hpp"

namespace Stand
{
#pragma pack(push, 1)
	class tbPositionSelection : public tbToggleableBehaviourScript
	{
	public:
		v3 pos{};

	protected:
		void onEnable() final;
		void onDisable() final;
		void onTick() final;
	};
#pragma pack(pop)

	inline tbPositionSelection g_tb_position_selection{};
}
