#pragma once

#include "PatternBatch.hpp"
#include "DetourHook.hpp"
#include "main.hpp" // g_gta_module

#define CODENAME(hn) hn##_hook.name

#define INIT_HOOK(name, flags) name##_hook.init(&name, p.as<void*>(), flags);
#define INIT_HOOK_MANDATORY(name) name##_hook.init(&name, p.as<void*>(), DH_MANDATORY);
#define INIT_HOOK_OPTIONAL(name) name##_hook.init(&name, p.as<void*>(), 0);
#define INIT_HOOK_OPTIONAL_LONGJUMP(name) name##_hook.init(&name, p.as<void*>(), DH_LONGJUMP);

#define INIT_HOOK_2(name, p, flags) name##_hook.init(&name, p.as<void*>(), flags);

#define BATCH_ADD_MANDATORY_HOOK(name, head_sig, body_sig) BATCH_ADD_FUNC_IMPL(CODENAME(name), name, head_sig, body_sig, add, INIT_HOOK_MANDATORY)
#define BATCH_ADD_OPTIONAL_HOOK(name, head_sig, body_sig) BATCH_ADD_FUNC_IMPL(CODENAME(name), name, head_sig, body_sig, add_optional, INIT_HOOK_OPTIONAL)
#define BATCH_ADD_OPTIONAL_LONGJUMP_HOOK(name, head_sig, body_sig) BATCH_ADD_FUNC_IMPL(CODENAME(name), name, head_sig, body_sig, add_optional, INIT_HOOK_OPTIONAL_LONGJUMP)

#undef STORE_POINTER
#define STORE_POINTER(name) name = p.as<decltype(name)>();

#define COMP_OG(name) name##_hook.getOriginal<decltype(&name)>()
