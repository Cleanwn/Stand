#pragma once

#include <optional>
#include <string>

#include "fwddecl.hpp"
#include "typedecl.hpp"

#include "GarbageCollector.hpp"
#include "rlSessionInfo.hpp"
#include "ToastConfig.hpp"

#define RG_PEERADDR false

namespace Stand
{
	enum RemoteGamerUpdateType : uint8_t
	{
		RGU_NONE = 0,
		RGU_BULK,
		RGU_DETAILED,
	};

#pragma pack(push, 1)
	class RemoteGamer : public GarbageCollected
	{
	public:
		enum OnlineStatus : uint8_t
		{
			TBD = 0,
			OFFLINE,
			STORY_MODE,
			ONLINE_INVITE,
			ONLINE_CLOSED_FRIEND,
			ONLINE_CLOSED_CREW,
			ONLINE_CREW,
			ONLINE_SOLO,
			ONLINE_PUBLIC,
			ONLINE_OTHER
		};

		static constexpr time_t transition_millis = 90000;
		static constexpr auto MAX_BULK_REQUESTS = 32;

		inline static GarbageCollector gc{};
		inline static std::vector<RemoteGamer*> tracked{};
		inline static ToastConfig tracking_update_toast_config{ true };
		inline static bool inject_tracked_info = true;

		const int64_t rid;
		bool fake_friend;

		bool response_from_detailed;
		time_t status_as_of = 0;
		time_t requested_at = 0;
		OnlineStatus online_status = TBD;
#if RG_PEERADDR
		rage::netPeerAddress peeraddr;
#else
		bool has_peeraddr;
#endif
		rage::rlSessionInfo session_info{};
		time_t story_since = 0;

		explicit constexpr RemoteGamer(int64_t rid)
			: rid(rid)
		{
		}

	public:
		[[nodiscard]] static RemoteGamer* getSimple(int64_t rid);
		[[nodiscard]] static RemoteGamer* get(int64_t rid, bool can_create, bool can_request_update) = delete;
		[[nodiscard]] static RemoteGamer* get(int64_t rid, bool can_create, RemoteGamerUpdateType update_type);
	private:
		[[nodiscard]] static RemoteGamer* create(int64_t rid, bool detailed);
	public:
		void considerRefresh(bool detailed);
	private:
		[[nodiscard]] bool canBeTracked() const noexcept;
		void request(bool detailed);
		void requestDetailed();
	public:
		static void onTickUltimate();
		static void onTick();

		[[nodiscard]] bool isUser() const noexcept;
		[[nodiscard]] bool isQueuedOrPending() const noexcept;
		[[nodiscard]] bool hasResponse() const noexcept;
		[[nodiscard]] bool hasPeerAddr() const noexcept;
		[[nodiscard]] bool isOnline() const noexcept;
		[[nodiscard]] bool hasSession() const noexcept;

		void processGamerState(const char* peeraddr, std::string&& gsinfo, int64_t gstype);
		void processGamerState(bool signedInToScPresence, std::string&& gsinfo, int64_t gstype);
	private:
		[[nodiscard]] bool setOnlineStatus(OnlineStatus online_status) noexcept;
		void onTrackUpdateSimple(const std::string& activity);
		void onTrackUpdateSession();
		static void onTrackUpdateSession(const ScAccount* a, std::string&& activity);
		static void onTrackUpdateNotify(std::string&& str);

	public:
		[[nodiscard]] std::string getSessionTypeString() const noexcept;
		[[nodiscard]] Label toShortString() const;
		[[nodiscard]] std::string toUserString(bool can_issue_requests) const;

		[[nodiscard]] std::string getGsInfo() const;
		[[nodiscard]] std::optional<int64_t> getGsType() const noexcept;

		[[nodiscard]] static bool haveTrackPermissions() noexcept;
	private:
		[[nodiscard]] bool isTrackedIgnorePermission() noexcept;
	public:
		[[nodiscard]] bool isTracked() noexcept;
		[[nodiscard]] static bool isAnyTracked() noexcept;
		void track() noexcept;
		void untrack() noexcept;

		bool useAsTrampoline(bool nuts);

		static void deinit();

	protected:
		bool canBeDeleted() noexcept final;
	};
#pragma pack(pop)
}
