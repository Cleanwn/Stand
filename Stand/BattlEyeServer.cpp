#include "BattlEyeServer.hpp"

#include <soup/base64.hpp>
#include <soup/ObfusString.hpp>

#include <fmt/core.h>

#include "AbstractPlayer.hpp"
#ifdef STAND_DEBUG
#include "FileLogger.hpp"
#endif

namespace Stand
{
	struct BattlEyeUserData
	{
		using log_func_t = void(*)(const char* msg, int level);
		using kick_player_t = void(*)(std::uint64_t id, const char* reason);
		using send_message_t = void(*)(std::uint64_t id, const void* msg, uint32_t size);

		const char* game_name;
		log_func_t log_func;
		kick_player_t kick_player;
		send_message_t send_message;
		void* unk;
	};

	struct BattlEyeApi
	{
		using shutdown_t = void(*)();
		using run_t = bool(*)();
		using run_command_t = void(*)(const char* command);
		using add_player_t = void(*)(uint64_t id, uint32_t ip_addr, uint16_t port, const char* name, char unused);
		using set_player_state_t = void(*)(uint64_t id, int reason);
		using assign_guid_t = void(*)(uint64_t id, const void* guid, uint32_t size);
		using receive_message_t = void(*)(uint64_t id, const void* data, uint32_t size);

		shutdown_t shutdown;
		run_t run;
		run_command_t run_command;
		add_player_t add_player;
		set_player_state_t set_player_state;
		assign_guid_t assign_guid;
		assign_guid_t assign_guid_verified;
		receive_message_t receive_message;
	};

	using init_t = bool(*)(int api_level, BattlEyeUserData* data, BattlEyeApi* api);

	static HMODULE s_hmod = NULL;
	static BattlEyeUserData s_user_data; // lifetime requirement is unclear to me, rather keep it static
	static BattlEyeApi s_api;

	bool BattlEyeServer::isRunning()
	{
		soup::ObfusString path("BattlEye\\BEServer_x64.dll");
		return GetModuleHandleA(path.c_str());
	}

	bool BattlEyeServer::isRunningDueToUs()
	{
		return s_hmod != NULL;
	}

	bool BattlEyeServer::start()
	{
		soup::ObfusString path("BattlEye\\BEServer_x64.dll");
		auto hmod = LoadLibraryA(path.c_str());
		SOUP_IF_UNLIKELY (hmod == NULL)
		{
			return false;
		}
		s_hmod = hmod;
		s_user_data.game_name = "paradise";
		s_user_data.log_func = [](const char* message, int level)
		{
#ifdef STAND_DEBUG
			g_logger.log(fmt::format(fmt::runtime(soup::ObfusString("[BattlEye] {}").str()), message));
#endif
		};
		s_user_data.kick_player = [](uint64_t peerid, const char* reason)
		{
#ifdef STAND_DEBUG
			g_logger.log(fmt::format(fmt::runtime(soup::ObfusString("BattlEye wants us to kick {} with reason \"{}\".").str()), AbstractPlayer::fromPeerId(peerid).getName(), reason));
#endif
			// If the clients saw a legit host in this session before us, they're gonna be uncooperative, and our server is gonna want them gone for "Client not responding" and soon after the clients will want us gone because our server is not talking to them anymore.
			// However, we can simply re-add them to our server, and they might even become a bit more cooperative.
			FiberPool::queueJob([peerid]()
			{
				if (auto p = AbstractPlayer::fromPeerId(peerid); p.isValid())
				{
					const auto name = p.getSessionName();
					BattlEyeServer::addPlayer(peerid, p.getRockstarId(), name.c_str());
				}
			});
		};
		s_user_data.send_message = [](uint64_t peerid, const void* data, uint32_t size)
		{
			//g_logger.log(fmt::format("Sending {} bytes to {}", size, AbstractPlayer::fromPeerId(peerid).getName()));
			AbstractPlayer::fromPeerId(peerid).sendBePacket(false, data, size);
		};
		s_user_data.unk = nullptr;
		SOUP_IF_UNLIKELY (!reinterpret_cast<init_t>(GetProcAddress(hmod, "Init"))(1, &s_user_data, &s_api))
		{
			return false;
		}
		for (const auto& p : AbstractPlayer::listExcept(true))
		{
			const auto name = p.getSessionName();
			BattlEyeServer::addPlayer(p.getPeerId(), p.getRockstarId(), name.c_str());
		}
		return true;
	}

	void BattlEyeServer::stop()
	{
		s_api.shutdown();

		FreeLibrary(s_hmod);
		s_hmod = NULL;
	}

	void BattlEyeServer::tick()
	{
		s_api.run();
	}

	void BattlEyeServer::addPlayer(uint64_t peerid, uint64_t rockstar_id, const char* name)
	{
		const auto guid = soup::base64::encode(fmt::to_string(rockstar_id));

		s_api.add_player(peerid, -1, 0, name, false);
		s_api.assign_guid(peerid, guid.data(), (uint32_t)guid.size());
		s_api.assign_guid_verified(peerid, guid.data(), (uint32_t)guid.size());
		s_api.set_player_state(peerid, 1);
	}

	void BattlEyeServer::removePlayer(uint64_t token)
	{
		s_api.set_player_state(token, -1);
	}

	void BattlEyeServer::receivePacket(uint64_t peerid, const void* data, uint32_t size)
	{
		s_api.receive_message(peerid, data, size);
	}
}
