#include "CodeIntegrity.hpp"

#include <soup/joaat.hpp>
#include <soup/x64.hpp>

namespace Stand
{
	struct CheckedFunction
	{
		void* func;
		uint32_t hash;
	};

	static std::vector<CheckedFunction> checked_funcs{};

	void CodeIntegrity::add(void* func)
	{
		checked_funcs.emplace_back(func, hashFunction(func));
	}

	bool CodeIntegrity::verify()
	{
		for (const auto& c : checked_funcs)
		{
			if (hashFunction(c.func) != c.hash)
			{
				return false;
			}
		}
		return true;
	}

	uint32_t CodeIntegrity::hashFunction(const void* func)
	{
		uint32_t hash = 0;
		auto p = (const uint8_t*)func;
		while (true)
		{
			auto start = p;
			auto ins = soup::x64Disasm(p);
			if (!ins.isValid())
			{
				break;
			}
			hash = soup::joaat::partial((const char*)start, p - start, hash);
			if (strcmp(ins.operation->name, "jmp") == 0)
			{
				p += (int32_t)ins.operands[0].val;
			}
			/*else if (strcmp(ins.operation->name, "call") == 0)
			{
				hashFunction(p + ins.operands[0].displacement);
			}*/
			else if (strcmp(ins.operation->name, "ret") == 0)
			{
				break;
			}
		}
		soup::joaat::finalise(hash);
		return hash;
	}
}
