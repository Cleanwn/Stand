#include "FileLogger.hpp"

#include <soup/ObfusString.hpp>

#include "ensure_folder.hpp"

namespace Stand
{
	std::wstring FileLogger::getMainFilePath()
	{
		std::wstring path = _wgetenv(L"appdata");
		path.append(L"\\Stand");
		ensure_folder(path);
		path.append(L"\\Log.txt");
		return path;
	}

	bool FileLogger::isInited() const
	{
		return out.operator bool();
	}

	void FileLogger::init(std::filesystem::path path)
	{
		const bool fresh = !std::filesystem::exists(path);
		out.open(std::move(path), std::ios_base::out | std::ios_base::app);
		if (fresh)
		{
			markWithBom();
		}
	}

	void FileLogger::markWithBom()
	{
		out << "\xEF\xBB\xBF";
		out.flush();
	}

	void FileLogger::empty()
	{
		if (out)
		{
			out.close();
			out.open(getMainFilePath(), std::ios_base::out);
			markWithBom();
			clearRepetitionData();
		}
	}

	void FileLogger::deinit()
	{
		out.close();
	}

	void FileLogger::write(std::string&& message)
	{
		out << message << std::flush;
	}
}
