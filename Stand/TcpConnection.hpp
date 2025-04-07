#pragma once

#include <string>

#include <soup/Scheduler.hpp>

namespace Stand
{
#pragma pack(push, 1)
	class TcpConnection : protected soup::Scheduler
	{
	public:
		[[nodiscard]] soup::Socket& getSocket() const noexcept;

		void close();

		[[nodiscard]] static int getLastError() noexcept;
		[[nodiscard]] static std::string errorCodeToString(int error_code) noexcept;

	private:
		void onSendError(int code);
	public:
		void sendRaw(const std::string& str);
	};
#pragma pack(pop)
}
