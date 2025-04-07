#pragma once

#include "tbToggleableBehaviour.hpp"

namespace Stand
{
#pragma pack(push, 1)
	class tbToggleableBehaviourScript : public tbToggleableBehaviour
	{
	private:
		bool handler_active = false;

	protected:
		void onEnable() override;

		virtual void onTick() = 0;
	};
#pragma pack(pop)
}
