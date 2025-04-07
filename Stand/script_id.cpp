#include "script_id.hpp"

#include "datBitBuffer.hpp"

namespace rage
{
	/*bool scriptIdBase::ser(datImportBuffer& bb)
	{
		Read(bb);
		return true;
	}*/

	bool scriptIdBase::ser(datExportBuffer& bb)
	{
		Write(bb);
		return true;
	}
}

void CGameScriptId::read(rage::datBitBuffer& buf)
{
	m_hash = buf.readU32();
	m_timestamp = buf.readU32();

	if (buf.readBool())
	{
		m_position_hash = buf.readU32();
	}

	if (buf.readBool())
	{
		instance_id = buf.readU8();
	}
}
