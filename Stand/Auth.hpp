#pragma once

#include <functional>
#include <string>

#include <soup/rsa.hpp>

#include "fwddecl.hpp"
#include "typedecl.hpp"

#include "GamerIdentifier.hpp"

namespace Stand
{
	using LicensePermissions_t = uint8_t;

	enum LicensePermissions : LicensePermissions_t
	{
		LICPERM_FREE = 0,
		LICPERM_BASIC = 1,
		LICPERM_REGULAR = 2,
		LICPERM_ULTIMATE = 3,
	};

	enum AccountUnlocks : uint8_t
	{
		UNLOCK_EARLY_TOXIC = 0b1,
		UNLOCK_PRE100 = 0b10,
		UNLOCK_USED100 = 0b100,
		UNLOCK_RNDBOOL = 0b1000,
	};

#define ACTIVATION_KEY_CHARS 31

#pragma pack(push, 1)
	class Auth
	{
	public:
		const char crash_key[ACTIVATION_KEY_CHARS + 1] = "es17qxgneczfy0uhyva7689sqdeqi8w";

		soup::RsaPublicKeyLonglived public_key;

		bool activation_notify = false;
		bool showed_connection_error_notify = false;
		std::string activation_key_to_try = {};
		bool activation_key_was_valid = false;

		LicensePermissions license_permissions = LICPERM_FREE;
		std::string perm_sig = {};
		unixtime_t perm_expiry = 0;

		bool sent_next_heartbeat = false;
		unixtime_t next_heartbeat = 0;

		uint64_t direct_packet_magic = 0;

		uint64_t remote_session = (uint64_t)-1;
		std::vector<soup::UniquePtr<GamerIdentifier>> remote_session_peers{};

		std::string root_name_container = {};
		uint8_t unlocks = 0;

		std::unordered_set<hash_t> once_messages_shown{};

		std::string showed_update_notify{};
		bool job_queued = false;

		bool stand_user_identification = true;
		bool stand_user_identification_this_session = true; // default to true so people can't reinject to bypass bad boy

		explicit Auth();

		[[nodiscard]] bool isStateConsistent() const;
		[[nodiscard]] bool isStateConsistentRaw() const;
		[[nodiscard]] __declspec(noinline) LicensePermissions verifyPermSig() const;
		[[nodiscard]] bool isLatestVersion() const noexcept;
	private:
		__declspec(noinline) bool processHeartbeatResponse(unixtime_t expiry, std::string&& data);
		void notifyInvalidActivationKey() const;
	public:
		bool discoverActivationKey();
		void enableNotifications() noexcept;
		void tryActivationKey();
		__declspec(noinline) void sendHeartbeat(uint64_t session);
		[[nodiscard]] bool isDifferentSession(uint64_t session) const;
		[[nodiscard]] bool isSuiEnabledThisSession() const noexcept;

		[[nodiscard]] __forceinline bool hasApiCredentials() const noexcept
		{
			return license_permissions != LICPERM_FREE
				&& !activation_key_to_try.empty()
				;
		}

		__declspec(noinline) void reportEvent(const Codename& type, const std::string& data = {});
	private:
		static void processEventResponse(soup::HttpResponse&& resp);

	public:
		static void logOutManual(Click& click);
		__forceinline void logOut(bool retry_activation);
	};
#pragma pack(pop)

	inline Auth g_auth{};
}
