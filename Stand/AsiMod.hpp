#pragma once

#include <string>

#include <Windows.h>

#include "fwddecl.hpp"

#include "AsiDirectory.hpp"

namespace Stand
{
#pragma pack(push, 1)
	class AsiMod
	{
	private:
		std::wstring temp_path{};
		HMODULE shv_h = nullptr;
	public:
		HMODULE asi_h = nullptr;
		CommandPhysical* cmd;
		const AsiDirectory dir;

		inline static bool use_temp_dir = false;

		explicit AsiMod(CommandPhysical* cmd, AsiDirectory dir);

		void initCommand();

		[[nodiscard]] std::wstring getPath(bool discrete = false) const;

	public:
		[[nodiscard]] bool isRunning() const noexcept;
		void load();
		void unload(bool notify);
	};
#pragma pack(pop)
}
