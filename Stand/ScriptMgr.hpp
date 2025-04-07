#pragma once

#include "natives_decl.hpp"

#include <memory>

#include "Script.hpp"
#include "VectorMap.hpp"
#include "XoredNativeHash.hpp"

#define USE_ENTRYPOINT_DETOURS true

namespace Stand
{
#pragma pack(push, 1)
	class ScriptMgr // may only be used from within the script thread
	{
	public:
		uint32_t script_thread = 0;
		std::unordered_map<XoredNativeHash, rage::scrNativeHandler> handler_map{};
		std::unordered_map<XoredNativeHash, rage::scrNativeHandler> call_map{};
		VectorMap<rage::scrNativeHandler, rage::scrNativeHash> reverse_entrypoint_map{};
	private:
		VectorMap<HMODULE, std::unique_ptr<Script>> scripts{};

	public:
		explicit ScriptMgr() = default;
		~ScriptMgr() = default;

		void addScript(HMODULE hmod, std::unique_ptr<Script>&& scr);
		[[nodiscard]] HMODULE getScriptModule(const Script* scr);
		void removeScripts(HMODULE hmod);
		void removeScript(script_func_t func);
		[[nodiscard]] size_t getNumScripts(HMODULE hmod) const;

		void tick();

		void initEntrypointMaps();
	private:
		void mapEntrypoint(rage::scrNativeHash og_hash, rage::scrNativeHash current_hash);
		void mapEntrypoint(rage::scrNativeHash og_hash, rage::scrNativeHandler entrypoint);
	public:
		void mapEntrypointReverse(rage::scrNativeHandler entrypoint, rage::scrNativeHash og_hash);
		void mapDetour(rage::scrNativeHash og_hash, rage::scrNativeHandler replacement, rage::scrNativeHandler* ogfpp = nullptr);

		void deinit();
	};
#pragma pack(pop)

	inline ScriptMgr g_script_mgr;
}
