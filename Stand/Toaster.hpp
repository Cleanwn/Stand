#pragma once

#include <string>

namespace Stand
{
	class Toaster
	{
	public:
		virtual void toast(const std::wstring& message) const = 0;

		virtual void setPersistentToast(const std::wstring& message) = 0;
		virtual void clearPersistentToast() = 0;
	};

	extern Toaster* g_toaster;
}
