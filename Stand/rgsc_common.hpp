#pragma once

#define RGSC_CALL __stdcall

using RGSC_HRESULT = int;

enum RgscReturnCodes
{
	RGSC_INVALIDARG		= 0x80070057L,	// Matches E_INVALIDARG
	RGSC_NOINTERFACE	= 0x80004002L,	// Matches E_NOINTERFACE
	RGSC_FAIL			= 0x80004005L,	// Matches E_FAIL
	RGSC_OK = 0L,						// Matches S_OK
};

struct RGSC_IID
{
	unsigned long  Data1;
	unsigned short Data2;
	unsigned short Data3;
	unsigned char  Data4[8];
};

using RGSC_REFIID = const RGSC_IID&;

inline bool operator==(RGSC_REFIID src1, RGSC_REFIID src2)
{
	return ((src1.Data1 == src2.Data1) &&
		(src1.Data2 == src2.Data2) &&
		(src1.Data3 == src2.Data3) &&
		(*(__int64*)src1.Data4 == *(__int64*)src2.Data4)) ? true : false;
}

namespace rgsc
{
	inline const RGSC_IID IID_IRgscUnknown = { 0x298e73ac, 0x3a30, 0x47a5, {0xb4, 0x50, 0xb9, 0x9a, 0x24, 0xbc, 0xc4, 0x27} };

	struct IRgscUnknown
	{
		virtual RGSC_HRESULT RGSC_CALL QueryInterface(RGSC_REFIID riid, void** ppvObject) = 0;
	};

	using RockstarId = int64_t;
}
