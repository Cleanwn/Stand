#pragma once

#include <cstdint>
#include <functional>
#include <string>

#include "fwddecl.hpp"

namespace Stand
{
	class ScAccount
	{
	public:
		enum RequestStatus : uint8_t
		{
			REQUEST_STATUS_NONE,
			REQUEST_STATUS_QUEUED,
			REQUEST_STATUS_DONE,
		};

		enum CacheStatus : uint8_t
		{
			CACHE_STATUS_NONE,
			CACHE_STATUS_PENDING,
			CACHE_STATUS_HIT,
			CACHE_STATUS_MISS,
		};

		int64_t rid = 0x8000000000000000;
		std::string name{};
		RequestStatus request_status = REQUEST_STATUS_NONE;
		CacheStatus cache_status = CACHE_STATUS_NONE;

		explicit ScAccount(int64_t rid);
		explicit ScAccount(const std::string& name);

		// profile pic: "https://a.rsg.sc/n/" name_lower_case (result is 128x128, can append "/s" for 64x64 but won't work for custom pictures)
		// character pic: "https://prod.ros.rockstargames.com/cloud/11/cloudservices/members/sc/" rid "/publish/gta5/mpchars/" char_slot ".png"
		// ^ redirects to: "https://prod.cloud.rockstargames.com/members/sc/6110/" rid "/publish/gta5/mpchars/" char_slot ".png"
		// crew pic: https://prod.cloud.rockstargames.com/crews/sc/6604/" crew_id "/publish/emblem/emblem_128.png"

		[[nodiscard]] bool hasRID() const noexcept;
		[[nodiscard]] bool hasName() const noexcept;
		[[nodiscard]] bool isValid() const noexcept;
		[[nodiscard]] bool isComplete() const noexcept;
		[[nodiscard]] bool isRemoteValid() const noexcept;

		[[nodiscard]] static ScAccount* fromRID(int64_t rid);
		[[nodiscard]] static ScAccount* fromName(const std::string& name);

		[[nodiscard]] const char* getNameLazy(bool rid_can_be_invalid = false) const noexcept;
		void requestNameLazy(std::function<void(const ScAccount&)>&& callback) noexcept;

		[[nodiscard]] std::string getNameForUser() const;
		[[nodiscard]] static std::string formatName(int64_t rid, std::string raw_name);
		[[nodiscard]] std::string getNameForUserAndCheckCompletion(bool can_issue_requests);

		static void rid2name(Click& click, int64_t rid, std::function<void(const ScAccount&)>&& success_callback);
		static bool name2rid(Click& click, const std::string& name, std::function<void(const ScAccount&)>&& success_callback);

	public:
		[[nodiscard]] static bool canRequestCompletionImmediately() noexcept;
		[[nodiscard]] bool wasCompletionRequested() noexcept;
		void requestCompletion();
		void requestCompletionPreferCache();
		void requestCompletion(std::function<void(const ScAccount*)>&& f);
		void requestCompletionPreferCache(std::function<void(const ScAccount*)>&& f);
	private:
		void requestCompletionImpl();
		void requestCompletionPreferCacheImpl();
	public:
		static void onTick() noexcept;
		[[nodiscard]] static std::string getAuthorizationHeaderValue() noexcept;
	private:
		void dispatchRequest() noexcept;
		void dispatch_rid2name(bool from_cache) noexcept;
		void dispatch_name2rid() noexcept;
		void onComplete() noexcept;
		void onCompleteFromCache() noexcept;
		void fireCompletionCallbacks() noexcept;

	public:
		[[nodiscard]] std::string toString(bool always_show_rid = true) const;

		static void deinit();
	};
}
