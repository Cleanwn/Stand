#pragma once

#include "TcpConnection.hpp"

#include <string>

#include "fwddecl.hpp"
#include "RecursiveSpinlock.hpp"

namespace Stand
{
#pragma pack(push, 1)
	class RelayCon : public TcpConnection
	{
	public:
		bool thread_running = false;
		HANDLE thread = INVALID_HANDLE_VALUE;

		std::string server{};

		[[nodiscard]] bool isRunning() const noexcept;
		void setServerAndInit();
		void init();

	private:
		std::string recv_message_buffer{};
		void run();
		void setRecvHandler(soup::Socket& s) SOUP_EXCAL;
		void processMessage(const std::string& message) SOUP_EXCAL;
		void sendHandshakeResponse();
		[[nodiscard]] Command* getCommand(const std::string& target);

	public:
		RecursiveSpinlock send_mtx;
		void sendLine(std::string&& str);
		void sendLineAsync(std::string&& str);

		void sendLang();

		void cleanup() noexcept;
	};
#pragma pack(pop)

	inline RelayCon g_relay{};
}
