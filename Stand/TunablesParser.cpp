#include "TunablesParser.hpp"

#include "natives.hpp"
#include "script_vm_common.hpp"
#include "sgnParseState.hpp"

namespace Stand
{
	bool TunablesParser::canQuery()
	{
		if (SCRIPT::HAS_SCRIPT_WITH_NAME_HASH_LOADED(ATSTRINGHASH("tuneables_processing")))
		{
			return true;
		}
		SCRIPT::REQUEST_SCRIPT_WITH_NAME_HASH(ATSTRINGHASH("tuneables_processing"));
		need_cleanup = true;
		return false;
	}

	uint32_t TunablesParser::queryOffset(hash_t hash)
	{
		auto prog = rage::scrProgram::fromHash(ATSTRINGHASH("tuneables_processing"));
		SOUP_ASSERT(prog);
		uint32_t res = queryOffset(prog, hash);
		if (need_cleanup)
		{
			need_cleanup = false;
			SCRIPT::SET_SCRIPT_WITH_NAME_HASH_AS_NO_LONGER_NEEDED(ATSTRINGHASH("tuneables_processing"));
		}
		return res;
	}

	uint32_t TunablesParser::queryOffset(rage::scrProgram* prog, hash_t hash)
	{
		ScriptGenie::ParseState ps(prog);
		while (ps.tick())
		{
			ps.advance();
			if (!ps.stack.empty()
				&& ps.stack.top() == hash
				)
			{
				while (ps.tick())
				{
					if (*ps.getCodePtr() == SetStruct1)
					{
						return ps.getCodePtr()[1];
					}
					if (*ps.getCodePtr() == SetStruct2)
					{
						return *(uint16_t*)(ps.getCodePtr() + 1);
					}
					ps.advance();
				}
			}
		}
		return 0;
	}
}
