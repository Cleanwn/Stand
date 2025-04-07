#pragma once

#include <cstdint>

namespace Stand
{
	using ScriptErrorRecoveryMode_t = uint8_t;

	struct ScriptErrorRecoveryMode
	{
		enum _ : ScriptErrorRecoveryMode_t
		{
			ENABLED,
			FREEMODE_ONLY,
			DISABLED,
		};
	};

	inline ScriptErrorRecoveryMode_t script_error_recovery_mode = ScriptErrorRecoveryMode::ENABLED;
}
