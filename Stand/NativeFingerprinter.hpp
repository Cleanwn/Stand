#pragma once

#include <map>
#include <unordered_map>

#include <soup/fwd.hpp>
#include <soup/x64.hpp>

#include "natives_decl.hpp"

namespace Stand
{
	struct NativeFingerprinter
	{
		static void normaliseIns(soup::Writer& w, const soup::x64Instruction& ins);
		static void normaliseFunc(soup::Writer& w, const uint8_t* p);
		[[nodiscard]] static uint32_t getFuncHash(const uint8_t* p);
		[[nodiscard]] static std::unordered_map<rage::scrNativeHash, uint32_t> getNativeFingerprints();
		[[nodiscard]] static std::unordered_map<uint32_t, rage::scrNativeHash> getUniqueNatives();
		static void write(soup::Writer& w, const std::unordered_map<rage::scrNativeHash, uint32_t>& native_fingerprints);
		static void write(soup::Writer& w, const std::unordered_map<uint32_t, rage::scrNativeHash>& unique_natives);

		[[nodiscard]] static std::map<uintptr_t, std::vector<rage::scrNativeHash>> getOrderedNatives();
		static void logOrderedNatives();
	};
}
