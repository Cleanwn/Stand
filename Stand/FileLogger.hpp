#pragma once

#include "Logger.hpp"

#include <filesystem>
#include <fstream>
#include <memory>

namespace Stand
{
	class FileLogger : public Logger
	{
	private:
		std::ofstream out{};

	public:
		[[nodiscard]] static std::wstring getMainFilePath();

		[[nodiscard]] bool isInited() const;
		void init(std::filesystem::path path);
	private:
		void markWithBom();
	public:
		void empty();
		void deinit();

	protected:
		void write(std::string&& message) final;
	};

	inline FileLogger g_logger{};
}
