#ifdef STAND_DEBUG

#include "NativeFingerprinter.hpp"

#include <unordered_set>

#include <fmt/format.h>

#include <soup/joaat.hpp>
#include <soup/StringWriter.hpp>

#include "conf.hpp"
#include "Exceptional.hpp"
#include "FileLogger.hpp"
#include "main.hpp"
#include "ScriptMgr.hpp"
#include "StringUtils.hpp"
#include "Util.hpp"

namespace Stand
{
	void NativeFingerprinter::normaliseIns(soup::Writer& w, const soup::x64Instruction& ins)
	{
		{
			uint16_t v = ins.operation->getUniqueId();
			w.u16(v);
		}
		for (uint8_t i = 0; i != ins.operation->getNumOperands(); ++i)
		{
			const soup::x64Operand& opr = ins.operands[i];
			if (opr.reg == soup::IMM)
			{
				uint64_t v = opr.val;
				w.u64(v);
			}
			else if (opr.reg != soup::DIS)
			{
				{
					uint8_t v = opr.access_type;
					w.u8(v);
				}
				{
					uint8_t v = opr.deref_size;
					w.u8(v);
				}
				/*{
					int32_t v = opr.deref_offset;
					w.i32(v);
				}*/
			}
		}
	}

	void NativeFingerprinter::normaliseFunc(soup::Writer& w, const uint8_t* p)
	{
		for (uint8_t i = 0; i != 35; ++i)
		{
			//drawDebugText((void*)p);
			soup::x64Instruction ins{};
			uint16_t status = -1;
			__try
			{
				ins = soup::x64Disasm(p);
			}
			//__except (EXCEPTION_EXECUTE_HANDLER)
			__EXCEPTIONAL()
			{
				status = -2;
				//drawDebugText("Failed to read code");
			}
			if (!ins.isValid())
			{
				w.u16(status);
				break;
			}
			if (strcmp(ins.operation->name, "jmp") == 0)
			{
				p += (int32_t)ins.operands[0].val;
			}
			else if (strcmp(ins.operation->name, "call") == 0)
			{
				normaliseIns(w, ins);
				uint32_t val = getFuncHash(p + ins.operands[0].displacement);
				//drawDebugText(fmt::format("Call at {} to function {}, hash {}", TO_IDA_ADDR(p), TO_IDA_ADDR(p + ins.operands[0].displacement), val));
				w.u32(val);
			}
			else if (strcmp(ins.operation->name, "nop") != 0)
			{
				normaliseIns(w, ins);
				//drawDebugText(ins.toString());
				if (strcmp(ins.operation->name, "ret") == 0)
				{
					break;
				}
			}
		}
	}

	uint32_t NativeFingerprinter::getFuncHash(const uint8_t* p)
	{
		soup::StringWriter sw(soup::ENDIAN_NATIVE);
		normaliseFunc(sw, p);
		return soup::joaat::hash(sw.data);
	}

	std::unordered_map<rage::scrNativeHash, uint32_t> NativeFingerprinter::getNativeFingerprints()
	{
		std::unordered_map<rage::scrNativeHash, uint32_t> native_fingerprints{};
		for (const auto& entry : g_script_mgr.handler_map)
		{
			native_fingerprints.emplace(entry.first.getHash(), getFuncHash((const uint8_t*)entry.second));
		}
		return native_fingerprints;
	}

	std::unordered_map<uint32_t, rage::scrNativeHash> NativeFingerprinter::getUniqueNatives()
	{
		std::unordered_map<uint32_t, rage::scrNativeHash> unique_natives{};
		std::unordered_set<uint32_t> ununique_func_hashes{};
		for (const auto& entry : g_script_mgr.handler_map)
		{
			const auto func_hash = getFuncHash((const uint8_t*)entry.second);
			if (ununique_func_hashes.contains(func_hash))
			{
				continue;
			}
			if (unique_natives.contains(func_hash))
			{
				unique_natives.erase(func_hash);
				ununique_func_hashes.emplace(func_hash);
				continue;
			}
			unique_natives.emplace(func_hash, entry.first.getHash());
			//g_logger.log(fmt::format("{} -> {}", Util::to_padded_hex_string((uint64_t)entry.first), Util::to_padded_hex_string(func_hash)));
			//unique_func_hashes.emplace(func_hash);
		}
		//g_logger.log(fmt::format("Out of {} natives, there are {} unique f-hashes, and {} unique ununique hashes.", g_script_mgr.entrypoint_map.size(), unique_func_hashes.size(), ununique_func_hashes.size()));
		return unique_natives;
	}

	void NativeFingerprinter::write(soup::Writer& w, const std::unordered_map<rage::scrNativeHash, uint32_t>& native_fingerprints)
	{
		for (const auto& entry : native_fingerprints)
		{
			uint64_t key = entry.first;
			uint32_t value = entry.second;
			w.u64(key);
			w.u32(value);
		}
	}

	void NativeFingerprinter::write(soup::Writer& w, const std::unordered_map<uint32_t, rage::scrNativeHash>& unique_natives)
	{
		for (const auto& entry : unique_natives)
		{
			uint32_t key = entry.first;
			uint64_t value = entry.second;
			w.u32(key);
			w.u64(value);
		}
	}

	std::map<uintptr_t, std::vector<rage::scrNativeHash>> NativeFingerprinter::getOrderedNatives()
	{
		std::map<uintptr_t, std::vector<rage::scrNativeHash>> ordered_natives{};
		for (const auto& entry : g_script_mgr.handler_map)
		{
			const auto k = GET_GTA_OFFSET(entry.second);
			auto e = ordered_natives.find(k);
			if (e == ordered_natives.end())
			{
				ordered_natives.emplace(k, std::vector<rage::scrNativeHash>{ entry.first.getHash() });
			}
			else
			{
				e->second.emplace_back(entry.first.getHash());
			}
		}
		return ordered_natives;
	}

	void NativeFingerprinter::logOrderedNatives()
	{
		for (const auto& e : NativeFingerprinter::getOrderedNatives())
		{
			std::string line{};
			for (const auto& native : e.second)
			{
				StringUtils::list_append(line, Util::to_padded_hex_string_with_0x(native));
			}
			line.insert(0, ": ");
			line.insert(0, fmt::to_string(e.first));
			g_logger.log(std::move(line));
		}
	}
}

#endif
