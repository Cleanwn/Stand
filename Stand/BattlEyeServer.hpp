#pragma once

namespace Stand
{
	struct BattlEyeServer
	{
		[[nodiscard]] static bool isRunning();
		[[nodiscard]] static bool isRunningDueToUs();
		static bool start();
		static void stop();
		static void tick();
		static void addPlayer(uint64_t peerid, uint64_t rockstar_id, const char* name);
		static void removePlayer(uint64_t peerid);
		static void receivePacket(uint64_t peerid, const void* data, uint32_t size);
	};
}
