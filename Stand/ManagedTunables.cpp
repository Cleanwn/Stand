#include "ManagedTunables.hpp"

#include "is_session.hpp"

namespace Stand
{
	int ManagedTunable::getRealValue()
	{
		if (has_real_value)
		{
			return real_value;
		}
		auto value = *ScriptGlobal(GLOBAL_TUNABLE_BASE).at(offset).as<int*>();
		real_value = value;
		if (is_session_started())
		{
			has_real_value = true;
		}
		return value;
	}

	void ManagedTunable::set(int value)
	{
		if (!has_real_value)
		{
			if (is_session_transition_active(true))
			{
				return;
			}
			real_value = *ScriptGlobal(GLOBAL_TUNABLE_BASE).at(offset).as<int*>();
			if (is_session_started())
			{
				has_real_value = true;
			}
		}
		*ScriptGlobal(GLOBAL_TUNABLE_BASE).at(offset).as<int*>() = value;
	}

	void ManagedTunable::reset() const
	{
		*ScriptGlobal(GLOBAL_TUNABLE_BASE).at(offset).as<int*>() = real_value;
	}
}
