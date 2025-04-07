#pragma once

namespace Stand
{
	inline void ensure_folder(const std::wstring& path)
	{
		if (!std::filesystem::exists(path))
		{
			std::filesystem::create_directory(path);
		}
		else if (!std::filesystem::is_directory(path))
		{
			std::filesystem::remove(path);
			std::filesystem::create_directory(path);
		}
	}
}
