#pragma once

#include <cstdint>

#include "gta_fwddecl.hpp"

#pragma pack(push, 1)
namespace rage
{
	class scriptIdBase
	{
	public:
		virtual ~scriptIdBase() = default;                            // 0 (0x00)

		// Assumes the script thread's identity.
		virtual void assume_thread_identity(scrThread*) {}; // 1 (0x08)

		// Returns whether the hash of the script id is valid.
		virtual bool is_valid() { return true; }; // 2 (0x10)

		// Gets the hash of the script id.
		virtual int32_t* get_hash(int32_t* out) { return nullptr; }; // 3 (0x18)

		// Gets an unknown value from the script id.
		virtual uint32_t* get_hash2(uint32_t* out) { return nullptr; }; // 4 (0x20)
		
		// Gets the name of the script id.
		virtual const char* get_name() { return ""; }; // 5 (0x28)

		// Serializes the script id from the buffer.
		virtual void Read(datBitBuffer& buffer) {}; // 6 (0x30)

		// Serializes the script id to the buffer.
		virtual void Write(datBitBuffer& buffer) {}; // 7 (0x38)
		
		// Calculates some information with the position hash & instand id.
		virtual uint32_t _0x40() { return 0;  }; // 8 (0x40)

		// Calls _0x40 and returns it's value added to another value.
		virtual uint32_t _0x48() { return 0; }; // 9 (0x48)

		// Logs some information about the script id.
		virtual void log_information(netLoggingInterface* logger) {}; // 10 (0x50)
		
		// Copies the information of other to this object.
		virtual void copy_data(scriptIdBase *other) {}                // 11 (0x58)
		
		// Returns whether the other script id is equal.
		virtual bool operator==(scriptIdBase* b) { return this == b; }; // 12 (0x60)

		virtual bool _0x68(void*) { return false; }; // 13 (0x68)

		bool ser(datImportBuffer& bb);
		bool ser(datExportBuffer& bb);
	};

	class scriptId : public scriptIdBase
	{
	public:
		int32_t m_hash;           // 0x08
		char m_name[0x20];        // 0x0C
	};
}

class CGameScriptId : public rage::scriptId
{
public:
	char m_padding[0x04];         // 0x2C
	int32_t m_timestamp;     // 0x30
	int32_t m_position_hash; // 0x34
	int32_t instance_id;     // 0x38
	int32_t m_unk;           // 0x3C

	void read(rage::datBitBuffer& buf);
};

static_assert(sizeof(CGameScriptId) == 0x40);
#pragma pack(pop)
