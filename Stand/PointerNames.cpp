#include "PointerNames.hpp"

#include <Windows.h>
#include <DbgHelp.h>

#include <fmt/format.h>

#include <soup/ObfusString.hpp>

#include "conf.hpp"
#include "Exceptional.hpp"
#include "main.hpp"
#include "mystackwalker.hpp"
#include "PeHeader.hpp"
#include "pointers.hpp"
#include "Util.hpp"

namespace Stand
{
	struct FunctionName
	{
		void* addr;
		std::string name;
	};
	
	class FunctionNameRegistry
	{
	private:
		bool sorted = true;
		std::vector<FunctionName> data{};

		void ensureSorted()
		{
			if (!sorted)
			{
				sorted = true;
				std::sort(data.begin(), data.end(), [](const FunctionName& a, const FunctionName& b)
				{
					return b.addr < a.addr;
				});
			}
		}

	public:
		[[nodiscard]] FunctionName* findAddrExact(void* addr) noexcept
		{
			for (auto& e : data)
			{
				if (e.addr == addr)
				{
					return &e;
				}
			}
			return nullptr;
		}

		void setName(void* addr, std::string name)
		{
			if (auto e = findAddrExact(addr))
			{
				[[unlikely]];
				//g_logger.log(fmt::format("Renaming function {} to {}", e->name, name));
				//e->name = std::move(name);
			}
			else
			{
				sorted = false;
				data.emplace_back(FunctionName{ addr, std::move(name) });
			}
		}

		[[nodiscard]] FunctionName* findAddrRough(void* addr) noexcept
		{
			ensureSorted();
			for (auto& e : data)
			{
				if (addr >= e.addr)
				{
					if ((reinterpret_cast<uintptr_t>(addr) - reinterpret_cast<uintptr_t>(e.addr)) <= 0x1000)
					{
						return &e;
					}
					break;
				}
			}
			return nullptr;
		}
	};

	static FunctionNameRegistry func_name_reg{};

	static DWORD64 dwDisplacement = 0;

	std::string PointerNames::format(void* addr)
	{
		std::string str = Util::to_padded_hex_string((uint64_t)addr);
		PeHeader::unveil();
		__try
		{
			HMODULE hmod;
			if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCWSTR)addr, &hmod) && hmod != nullptr)
			{
				bool use_debug_symbols = true;
				const std::string file_name = Util::get_module_file_name(hmod);
				auto offset = uint64_t(addr) - uint64_t(hmod);
				str.append(" (");
				if (file_name != g_stand_dll_file_name)
				{
					str.append(file_name).push_back('+');
					str.append(Util::to_padded_hex_string(offset));
				}
				if (file_name == "GTA5.exe")
				{
					str.append(", ").append(Util::to_padded_hex_string(offset + GTA_DUMP_BASE));
				}
				else if (file_name == g_stand_dll_file_name)
				{
					str.append(soup::ObfusString(STAND_NAMEVERSION).str()).append(", ").append(Util::to_padded_hex_string(offset + 0x1337694200000000));
#ifndef STAND_DEBUG
					use_debug_symbols = false;
#endif
				}
				str.push_back(')');
				if (auto name = getName(addr, use_debug_symbols); !name.empty())
				{
					str.append(": ").append(name);
				}
			}
		}
		__EXCEPTIONAL()
		{
		}
		PeHeader::veil();
		return str;
	}

	std::string PointerNames::getName(void* addr, bool use_debug_symbols)
	{
		std::string name{};
		if (auto e = func_name_reg.findAddrRough(addr))
		{
			name = e->name;
		}
		if (use_debug_symbols)
		{
			{
				IMAGEHLP_SYMBOL* sym = (IMAGEHLP_SYMBOL*)alloca(sizeof(IMAGEHLP_SYMBOL) + MyStackWalker::getMaxNameLength());
				sym->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);
				sym->MaxNameLength = MyStackWalker::getMaxNameLength();
				if (SymGetSymFromAddr64(MyStackWalker::getInstance()->getProcessHandle(), (DWORD64)addr, &dwDisplacement, sym))
				{
					if (!name.empty())
					{
						name.append(", ");
					}
					name.append((const char*)sym->Name);
				}
			}
			{
				IMAGEHLP_LINE64* data = (IMAGEHLP_LINE64*)alloca(sizeof(IMAGEHLP_LINE64));
				data->SizeOfStruct = sizeof(IMAGEHLP_LINE64);
				static_assert(sizeof(DWORD) <= sizeof(DWORD64));
				if (SymGetLineFromAddr64(MyStackWalker::getInstance()->getProcessHandle(), (DWORD64)addr, (PDWORD)&dwDisplacement, data))
				{
					name.append(" in ").append(data->FileName).append(" on line ").append(fmt::to_string(data->LineNumber));
				}
			}
		}
		return name;
	}

	void PointerNames::registerFunctionName(void* addr, std::string name)
	{
		func_name_reg.setName(addr, name);
	}
}
