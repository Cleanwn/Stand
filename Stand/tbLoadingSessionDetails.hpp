#pragma once

#include "tbToggleableBehaviour.hpp"

namespace Stand
{
	class tbLoadingSessionDetails : public tbToggleableBehaviour
	{
	protected:
		void onEnable() final;
		void onDisable() final;
	};

	inline tbLoadingSessionDetails g_tb_loading_session_details{};
}
