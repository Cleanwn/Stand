#include "CTunables.hpp"

#include <soup/sha1.hpp>

#include "pointers.hpp"

#ifdef STAND_DEBUG
#include "conf.hpp" // IDA_ADDR
#include "main.hpp" // IDA_ADDR
#endif

using namespace Stand;

static void sha_update(std::string& blob, int value)
{
	blob.append(std::string((const char*)&value, 4));
}

#ifdef STAND_DEBUG
CTunables& CTunables::GetInstance()
{
	return **(CTunables**)IDA_ADDR(00007FF765235BC0); // Search 0x9C6448A0, pointer to tunables instance is before it
}
#endif

void CTunables::disableRtmaChecks()
{
	for (auto& memcheck : memory_checks)
	{
		uint32_t xorValue = memcheck.nValue.Get() ^ MemoryCheck::MAGIC_XOR_VALUE;
		uint32_t flags = memcheck.nFlags.Get() ^ xorValue;

		//drawDebugText(flags);

		if (!(flags & 0x80000000)) // FLAG_MP_Processed
		{
			flags |= 0x80000000;
			memcheck.nFlags.Set(flags ^ xorValue);
			memcheck.updateXorCrc();
		}
	}
}

std::string CTunables::getVerifyHash() const
{
	std::string blob{};
	sha_update(blob, atoi(pointers::build_version)); // CDebug::GetVersionNumber()
	for (const auto& memcheck : memory_checks)
	{
		unsigned int x = memcheck.nVersionAndType.Get();
		uint32_t xorValue = memcheck.nValue.Get() ^ MemoryCheck::MAGIC_XOR_VALUE;
		sha_update(blob, x);
		x = memcheck.nAddressStart.Get();
		sha_update(blob, x);
		x = memcheck.nSize.Get();
		sha_update(blob, x);
		x = memcheck.nValue.Get();
		sha_update(blob, x);
		x = (memcheck.nFlags.Get() ^ xorValue) & 0xFFFFFF;
		sha_update(blob, x);
	}
	sha_update(blob, memory_checks.size());
	return soup::sha1::hash(blob);
}

#ifdef STAND_DEBUG
void CTunables::updateVerifyHash()
{
	auto TunablesVerifier_hash = *(void**)IDA_ADDR(00007FF789A54518); // This is valid for b2699 EGS. It's entirely within obfuscated code so good luck with that...
	auto current_hash = getVerifyHash();
	memcpy(TunablesVerifier_hash, current_hash.data(), current_hash.size());
}
#endif
