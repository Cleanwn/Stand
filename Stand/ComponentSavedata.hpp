#pragma once

#include "Component.hpp"

#ifdef STAND_DEBUG
#include <soup/JsonObject.hpp>
#endif

namespace Stand
{
	struct ComponentSavedata : public Component
	{
		void addPatterns(PatternBatch& batch) final;
		[[nodiscard]] std::vector<DetourHook*> getHooks() final;

		[[nodiscard]] sStatData* findStat(hash_t hash) const noexcept;

#ifdef STAND_DEBUG
		bool replay = false;
		bool record = false;
		soup::JsonObject obj;
#endif

		bool ignore_dirty_read = false;
	};
	inline ComponentSavedata g_comp_savedata;
}
