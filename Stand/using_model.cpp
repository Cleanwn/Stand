#include "using_model.hpp"

#include "atStringHash.hpp"
#include "Label.hpp"
#include "natives.hpp"
#include "Script.hpp"
#include "ThreadContext.hpp"
#include "Util.hpp"

namespace Stand
{
	bool using_model_auto(const hash_t hash, std::function<void()>&& callback)
	{
		const bool has_loaded = STREAMING::HAS_MODEL_LOADED(hash);
		uint8_t tally = 0;
		if (!has_loaded)
		{
			STREAMING::REQUEST_MODEL(hash);
			while (!STREAMING::HAS_MODEL_LOADED(hash))
			{
				if (++tally == 0xFF)
				{
					return false;
				}
				Script::current()->yield();
			}
		}
		std::invoke(std::move(callback));
		if (!has_loaded)
		{
			STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(hash);
		}
		return true;
	}

	void using_model(const hash_t hash, std::function<void()>&& callback)
	{
		if (!using_model_auto(hash, std::move(callback)))
		{
			Util::toast(LOC("GENFAIL"));
		}
	}
}
