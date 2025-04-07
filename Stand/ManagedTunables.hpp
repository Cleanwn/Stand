#pragma once

#include <utility>

#include "ScriptGlobal.hpp"

namespace Stand
{
	class ManagedTunable
	{
	private:
		const int offset;
		int real_value;
	public:
		bool has_real_value = false;

		constexpr ManagedTunable(int&& offset)
			: offset(std::move(offset))
		{
		}

		int getRealValue();
		void set(int value);
		void reset() const;
	};

	struct ManagedTunables
	{
		inline static ManagedTunable snow = GLOBAL_TUNABLE_SNOW;
		inline static ManagedTunable peyotes = GLOBAL_TUNABLE_PEYOTES;
		inline static ManagedTunable orbital_cannon_cooldown = GLOBAL_ORBITAL_CANNON_COOLDOWN;
	};
}
