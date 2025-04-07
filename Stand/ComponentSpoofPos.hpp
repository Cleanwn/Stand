#pragma once

#include "Component.hpp"

#include "vector.hpp"

namespace Stand
{
	struct ComponentSpoofPos : public Component
	{
		void addPatterns(PatternBatch& batch) final;
		[[nodiscard]] std::vector<DetourHook*> getHooks() final;

		bool spoof_position = false;
		v3 spoofed_position{ 467.1256f, 5543.1304f, 785.0175f };
	};
	inline ComponentSpoofPos g_comp_spoofpos;
}
