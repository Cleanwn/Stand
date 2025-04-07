#pragma once

namespace Stand
{
	struct MsvcStdString
	{
		union
		{
			struct
			{
				char* data;
				uintptr_t seems_unused_without_sso;
			};
			char sso_buffer[16];
		};
		size_t size;
		size_t capacity;
	};
	static_assert(sizeof(MsvcStdString) == 0x20);
}
