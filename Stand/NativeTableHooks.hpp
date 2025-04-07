#pragma once

#include "hashtype.hpp"
#include "natives_decl.hpp"

#include "XoredNativeHash.hpp"

namespace Stand
{
	class NativeTableHooks
	{
	public:
		static void onTick();

		[[nodiscard]] static rage::scrNativeHandler og(XoredNativeHash native);

		static void createNativeEntrypointOverride(XoredNativeHash native, rage::scrNativeHandler entrypoint_override);
		static void createNativeEntrypointOverride(hash_t script, XoredNativeHash native, rage::scrNativeHandler entrypoint_override);

		static void removeNativeEntrypointOverride(XoredNativeHash native);
		static void removeNativeEntrypointOverride(XoredNativeHash native, rage::scrNativeHandler og_entrypoint);
		static void removeNativeEntrypointOverride(hash_t script, XoredNativeHash native);

		static void forceCleanup();
	};
}
