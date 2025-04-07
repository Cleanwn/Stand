#pragma once

#include <functional>

#include "typedecl.hpp"

namespace Stand
{
	// needs a yieldable script thread
	extern bool using_model_auto(const hash_t hash, std::function<void()>&& callback);
	extern void using_model(const hash_t hash, std::function<void()>&& callback);
}
