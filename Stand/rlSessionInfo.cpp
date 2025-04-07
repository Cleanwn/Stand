#include "rlSessionInfo.hpp"

#include <Windows.h>

#include <soup/base64.hpp>
#include <soup/joaat.hpp>

#include "datBitBuffer.hpp"
#include "pointers.hpp"

namespace rage
{
	using namespace Stand;

	rlSessionInfo::rlSessionInfo(const std::string& gsinfo)
	{
		fromGsInfo(gsinfo);
	}

	void rlSessionInfo::fromGsInfo(const std::string& gsinfo)
	{
		auto bin = soup::base64::decode(gsinfo);
		fromBinary(bin.data(), (uint32_t)bin.size());
	}

	void rlSessionInfo::fromBinary(void* data, uint32_t size_bytes)
	{
		datImportBuffer buf{};
		buf.SetReadOnlyBytes(data, size_bytes);
		//ser(buf);
		pointers::rage_datImport_netPeerAddress(buf, host);
		id = buf.readU64();
		token = buf.readU64();
	}

	void rlSessionInfo::reset()
	{
		ZeroMemory(this, sizeof(*this));
	}

	Stand::SessionId rlSessionInfo::getId() const
	{
		SessionId id{};
		id.id = this->id;
		id.token = this->token;
		id.host_rid = this->host.gamer_handle.rockstar_id;
		return id;
	}

	uhash_t rlSessionInfo::getIdHash() const
	{
		auto id = getId();
		return soup::joaat::hashRange((const char*)&id, sizeof(SessionId));
	}

	std::string rlSessionInfo::toGsinfo() const
	{
		auto cpy = *this;
		cpy.host.censorOnlineIp();
		return cpy.toBase64();
	}

	std::string rlSessionInfo::toBase64() const
	{
		char bin[200];
		/*datExportBuffer bb;
		bb.SetReadWriteBytes(bin, sizeof(bin));
		const_cast<rlSessionInfo*>(this)->ser(bb);
		return soup::base64::encode(bin, bb.GetNumBytesWritten(), true);*/
		ZeroMemory(bin, 200);
		unsigned int i;
		if (pointers::rage_netPeerAddress_Export(&host, bin, 200, &i))
		{
			auto dp = &bin[i];
			datBitBuffer::static_writeU64(dp, id);
			datBitBuffer::static_writeU64(dp, token);
			return soup::base64::encode(bin, i + 16, true);
		}
		return {};
	}
}
