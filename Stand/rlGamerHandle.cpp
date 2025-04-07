#include "rlGamerHandle.hpp"

#include "datBitBuffer.hpp"
#include "str2int.hpp"

namespace rage
{
	using namespace Stand;

	rlGamerHandle rlGamerHandle::fromString(const char* c) noexcept
	{
		rlGamerHandle res{};
		if (c[0] == 'S' && c[1] == 'C' && c[2] == ' ')
		{
			auto opt = str2int<int64_t>(c + 3);
			if (opt.has_value())
			{
				res = opt.value();
			}
		}
		return res;
	}

	void rlGamerHandle::Import(void* data) noexcept
	{
		datImportBuffer bb{};
		bb.SetReadOnlyBytes(data, 13);
		ser(bb);
	}

#ifdef STAND_DEBUG
	bool rlGamerHandle::write(datBitBuffer& bb) const
	{
		char buf[13];
		datExportBuffer innerBb;
		innerBb.SetReadWriteBytes(buf, 13);
		const_cast<rlGamerHandle*>(this)->ser(innerBb);
		return bb.WriteBytes(buf, innerBb.GetNumBytesWritten());
	}
#endif

	rlGamerHandle rlGamerHandle::fromScript(void* data) noexcept
	{
		datBitBuffer buf{};
		buf.SetReadOnlyBytes(data, 13);
		rlGamerHandle gh;
		gh.Import(data);
		return gh;
	}
}
