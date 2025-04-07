#include "ScAccount.hpp"

#include <queue>

#include <fmt/core.h>
#include <rapidjson/document.h>

#include <soup/json.hpp>
#include <soup/UniquePtr.hpp>

#include "Auth.hpp"
#include "Click.hpp"
#include "Hooking.hpp"
#include "HttpRequestBuilder.hpp"
#include "is_rid.hpp"
#include "name_validation.hpp"
#include "PlayerHistory.hpp"
#include "pointers.hpp"
#include "RateLimit.hpp"
#include "rlGamerInfo.hpp"
#include "rlRosCredentials.hpp"
#include "rockstar_admins.hpp"
#include "StringUtils.hpp"
#include "UnicodePrivateUse.hpp"
#include "Util.hpp"
#include "VectorMap.hpp"

namespace Stand
{
	static RateLimit ratelimit{ 30000, 10 };
	static std::vector<soup::UniquePtr<ScAccount>> sc_accounts{};
	static std::queue<ScAccount*> request_queue{};
	static VectorMap<ScAccount*, std::function<void(const ScAccount*)>> sc_account_callbacks{};

	ScAccount::ScAccount(int64_t rid)
		: rid(rid)
	{
	}

	ScAccount::ScAccount(const std::string& name)
		: name(name)
	{
	}

	bool ScAccount::hasRID() const noexcept
	{
		return rid != 0x8000000000000000;
	}

	bool ScAccount::hasName() const noexcept
	{
		return !name.empty();
	}

	bool ScAccount::isValid() const noexcept
	{
		return hasRID() || hasName();
	}

	bool ScAccount::isComplete() const noexcept
	{
		return request_status == REQUEST_STATUS_DONE;
	}

	bool ScAccount::isRemoteValid() const noexcept
	{
		return hasRID() && hasName();
	}

	ScAccount* ScAccount::fromRID(int64_t rid)
	{
		for (const auto& a : sc_accounts)
		{
			if (a->rid == rid)
			{
				return a.get();
			}
		}
		return sc_accounts.emplace_back(soup::make_unique<ScAccount>(rid)).get();
	}

	ScAccount* ScAccount::fromName(const std::string& name)
	{
		if (!is_name_valid(name))
		{
			return nullptr;
		}
		for (const auto& a : sc_accounts)
		{
			if (a->name == name)
			{
				return a.get();
			}
		}
		return sc_accounts.emplace_back(soup::make_unique<ScAccount>(name)).get();
	}

	const char* ScAccount::getNameLazy(bool rid_can_be_invalid) const noexcept
	{
		if (rid_can_be_invalid || is_rid_valid(rid))
		{
			if (hasName())
			{
				return name.c_str();
			}
			if (auto hp = PlayerHistory::findRID(rid))
			{
				return hp->name.c_str();
			}
		}
		return nullptr;
	}

	void ScAccount::requestNameLazy(std::function<void(const ScAccount&)>&& callback) noexcept
	{
		auto name = getNameLazy(false);
		if (name != nullptr)
		{
			return callback(*this);
		}
		requestCompletion([callback{std::move(callback)}](const ScAccount* a)
		{
			callback(*a);
		});
	}

	std::string ScAccount::getNameForUser() const
	{
		const char* raw_name = getNameLazy(true);
		if (raw_name == nullptr)
		{
			return {};
		}
		return formatName(rid, raw_name);
	}

	std::string ScAccount::formatName(int64_t rid, std::string raw_name)
	{
		if (is_admin_rid(rid))
		{
			std::string name = StringUtils::utf16_to_utf8(UnicodePrivateUse::rs);
			name.push_back(' ');
			name.append(raw_name);
			return name;
		}
		return raw_name;
	}

	std::string ScAccount::getNameForUserAndCheckCompletion(bool can_issue_requests)
	{
		std::string name{};
		const char* raw_name = nullptr;
		bool is_sc_name = true;
		if (hasName())
		{
			raw_name = this->name.c_str();
		}
		else
		{
			if (auto hp = PlayerHistory::findRID(rid))
			{
				raw_name = hp->name.c_str();
				is_sc_name = (hp->flags & HP_SCNAME);
			}
		}
		if (raw_name != nullptr)
		{
			if (is_admin_rid(rid))
			{
				if (!is_sc_name && can_issue_requests)
				{
					requestCompletion();
				}
				name = StringUtils::utf16_to_utf8(UnicodePrivateUse::rs);
				name.push_back(' ');
			}
			name.append(raw_name);
		}
		return name;
	}

	void ScAccount::rid2name(Click& click, int64_t rid, std::function<void(const ScAccount&)>&& success_callback)
	{
		ScAccount* a = ScAccount::fromRID(rid);
		if (a->isComplete())
		{
			if (a->hasName())
			{
				success_callback(*a);
				return;
			}
			click.setResponse(LOC("R2N_E"));
		}
		else
		{
			a->requestCompletion([success_callback{ std::move(success_callback) }](const ScAccount* a)
			{
				if (a->isRemoteValid())
				{
					success_callback(*a);
				}
				else
				{
					Util::toast(LOC("R2N_E"), TOAST_DEFAULT);
				}
			});
			click.setResponse(LOC("R2N_Q"));
		}
	}

	bool ScAccount::name2rid(Click& click, const std::string& name, std::function<void(const ScAccount&)>&& success_callback)
	{
		ScAccount* a = ScAccount::fromName(std::move(name));
		if (a == nullptr)
		{
			if (!name.empty())
			{
				click.setResponse(LOC("N2R_I"));
			}
			return true;
		}
		if (a->isComplete())
		{
			if (a->hasRID())
			{
				success_callback(*a);
				return false;
			}
			click.setResponse(LOC("N2R_E"));
		}
		else
		{
			a->requestCompletion([success_callback{ std::move(success_callback) }](const ScAccount* a)
			{
				if (a->isRemoteValid())
				{
					success_callback(*a);
				}
				else
				{
					Util::toast(LOC("N2R_E"), TOAST_DEFAULT);
				}
			});
			click.setResponse(LOC("N2R_Q"));
		}
		return false;
	}

	bool ScAccount::canRequestCompletionImmediately() noexcept
	{
		return request_queue.empty() && ratelimit.canRequest();
	}

	bool ScAccount::wasCompletionRequested() noexcept
	{
		return request_status != REQUEST_STATUS_NONE
			|| cache_status == CACHE_STATUS_PENDING
			;
	}

	void ScAccount::requestCompletion()
	{
		if (!isComplete() && isValid())
		{
			requestCompletionImpl();
		}
	}

	void ScAccount::requestCompletionPreferCache()
	{
		if (!isComplete() && isValid())
		{
			requestCompletionPreferCacheImpl();
		}
	}

	void ScAccount::requestCompletion(std::function<void(const ScAccount*)>&& f)
	{
		if (isComplete())
		{
			f(this);
		}
		else if (isValid())
		{
			sc_account_callbacks.emplace(this, std::move(f));
			requestCompletionImpl();
		}
	}

	void ScAccount::requestCompletionPreferCache(std::function<void(const ScAccount*)>&& f)
	{
		if (isComplete())
		{
			f(this);
		}
		else if (isValid())
		{
			sc_account_callbacks.emplace(this, std::move(f));
			requestCompletionPreferCacheImpl();
		}
	}

	void ScAccount::requestCompletionImpl()
	{
		if (!wasCompletionRequested())
		{
			request_status = REQUEST_STATUS_QUEUED;
			request_queue.emplace(this);
		}
	}

	void ScAccount::requestCompletionPreferCacheImpl()
	{
		if (!wasCompletionRequested())
		{
			if (hasRID())
			{
				dispatch_rid2name(true);
			}
			else
			{
				// Don't have a cache path for name2rid yet
				request_status = REQUEST_STATUS_QUEUED;
				request_queue.emplace(this);
			}
		}
	}

	void ScAccount::onTick() noexcept
	{
		if (!request_queue.empty() && ratelimit.canRequest())
		{
			request_queue.front()->dispatchRequest();
			request_queue.pop();
		}
	}

	std::string ScAccount::getAuthorizationHeaderValue() noexcept
	{
		std::string str{ "SCAUTH val=\"" };
		str.append(pointers::ros_credentials->scauth_token).push_back('\"');
		return str;
	}

	void ScAccount::dispatchRequest() noexcept
	{
		ratelimit.addRequest();
		if (hasRID())
		{
			dispatch_rid2name(false);
		}
		else
		{
			dispatch_name2rid();
		}
	}

	void ScAccount::dispatch_rid2name(bool from_cache) noexcept
	{
		if (!is_rid_valid(this->rid))
		{
			this->onComplete();
			return;
		}
		if (g_hooking.getRealRid() == rid)
		{
			this->name = pointers::rlPresence_GamerPresences_0_GamerInfo->name;
			this->onComplete();
			return;
		}
		if (!from_cache)
		{
			auto b = HttpRequestBuilder{ HttpRequestBuilder::POST, "scui.rockstargames.com", "/api/friend/getprofile" }
			.addHeader("Authorization", getAuthorizationHeaderValue())
			.addHeader("X-Requested-With", "XMLHttpRequest") // needed to avoid "CSRF" error
			.addHeader("Content-Type", "application/json");
			b.setPayload(std::move(std::string(R"({"RockstarId":)").append(fmt::to_string(rid)).append("}")));
			b.setResponseCallback([this](soup::HttpResponse&& resp)
			{
				rapidjson::Document document;
				if (!document.Parse(resp.body.data(), resp.body.size()).HasParseError() && document.IsObject())
				{
					auto mem_accounts = document.FindMember("Accounts");
					if (mem_accounts != document.MemberEnd()
						&& mem_accounts->value.IsArray()
						)
					{
						for (const auto& account : mem_accounts->value.GetArray())
						{
							this->name = account["RockstarAccount"]["Name"].GetString();
							break;
						}
					}
				}
				this->onComplete();
			})
			.dispatch();
		}
		else
		{
			this->cache_status = CACHE_STATUS_PENDING;
			std::string path = "/r/";
			path.append(fmt::to_string(rid));
			auto b = HttpRequestBuilder{ HttpRequestBuilder::GET, "sc-cache.stand.sh", std::move(path) };
			b.setResponseCallback([this](soup::HttpResponse&& resp)
			{
				if (resp.status_code == 200)
				{
					auto root = soup::json::decode(resp.body);
					this->name = root->asArr().at(0).asObj().at("name").asStr();
					this->request_status = REQUEST_STATUS_DONE; // make isComplete return true
					this->cache_status = CACHE_STATUS_HIT;
				}
				else
				{
					this->cache_status = CACHE_STATUS_MISS;
				}
				this->onCompleteFromCache();
			})
			.setFailCallback([this](std::string&&)
			{
				this->cache_status = CACHE_STATUS_MISS;
				this->onCompleteFromCache();
			})
			.dispatch();
		}
	}

	void ScAccount::dispatch_name2rid() noexcept
	{
		if (name == pointers::rlPresence_GamerPresences_0_GamerInfo->name)
		{
			this->rid = pointers::rlPresence_GamerPresences_0_GamerInfo->getHandle().rockstar_id;
			this->onComplete();
			return;
		}
		auto b = HttpRequestBuilder{ HttpRequestBuilder::POST, "scui.rockstargames.com", "/api/friend/accountsearch" }
		.addHeader("Authorization", getAuthorizationHeaderValue())
		.addHeader("X-Requested-With", "XMLHttpRequest") // needed to avoid "CSRF" error
		.addHeader("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8");
		b.setPayload(std::move(std::string("searchNickname=").append(name)));
		b.setResponseCallback([this](soup::HttpResponse&& resp)
		{
			rapidjson::Document document;
			if (!document.Parse(resp.body.data(), resp.body.size()).HasParseError() && document.IsObject())
			{
				auto mem_accounts = document.FindMember("Accounts");
				if (mem_accounts != document.MemberEnd()
					&& mem_accounts->value.IsArray()
					)
				{
					std::string searchname{ this->name };
					StringUtils::to_lower(searchname);
					for (const auto& account : mem_accounts->value.GetArray())
					{
						std::string nickname = account["Nickname"].GetString();
						std::string nickname_lower{ nickname };
						StringUtils::to_lower(nickname_lower);
						if (nickname_lower == searchname)
						{
							this->name = std::move(nickname);
							this->rid = account["RockstarId"].GetInt64();
							/* Also on the account:
								"Crew": {
									"Id":49644426,
									"Name":"I like cones",
									"Tag":"cone",
									"Color":"#3f02ff",
									"RankOrder":0
								}
							* We don't (always) get this info via rid2name...
							* Capitalization in "Tag" seems to be exactly as entered on the form, but is always shown in upper-case.
							* RankOrder: 0 = Leader. It might not be sequential.
							* 
							* Alternative APIs (unautenticated but require X-Requested-With: XMLHttpRequest):
							* - https://scapi.rockstargames.com/crew/ranksWithMembership?crewId=67012648&onlineService=sc&searchTerm=&memberCountToRetrieve=1000
							* - https://scapi.rockstargames.com/crew/byname?name=dudx
							*/
							break;
						}
					}
				}
			}
			this->onComplete();
		})
		.dispatch();
	}

	void ScAccount::onComplete() noexcept
	{
		request_status = REQUEST_STATUS_DONE;
		fireCompletionCallbacks();
		if (isRemoteValid())
		{
			auto hp = PlayerHistory::findRID(rid);
			if (hp != nullptr)
			{
				if (hp->name != name
					|| !(hp->flags & HP_SCNAME)
					)
				{
					hp->name = name;
					hp->flags |= HP_SCNAME;
					PlayerHistory::save();
				}
			}
			if (is_rid_valid(rid))
			{
				soup::JsonObject obj{};
				obj.add("i", rid);
				obj.add("n", name);
				g_auth.reportEvent("RS", obj.encode());
			}
		}
	}

	void ScAccount::onCompleteFromCache() noexcept
	{
		if (cache_status == CACHE_STATUS_MISS)
		{
#ifdef STAND_DEBUG
			Util::toast(fmt::format("rid2name cache miss for {}", rid));
#endif
			requestCompletion();
		}
		else
		{
			fireCompletionCallbacks();
		}
	}

	void ScAccount::fireCompletionCallbacks() noexcept
	{
		auto callbacks = sc_account_callbacks.find(this);
		if (callbacks != sc_account_callbacks.end())
		{
			for (const auto& f : callbacks->second)
			{
				f(this);
			}
			sc_account_callbacks.erase(callbacks);
		}
	}

	std::string ScAccount::toString(bool always_show_rid) const
	{
		std::string str{};
		if (hasName())
		{
			str = name;
		}
		else if (auto lz = getNameLazy())
		{
			str = lz;
		}
		if (!str.empty())
		{
			if (always_show_rid)
			{
				if (hasRID())
				{
					str.append(" (");
					str.append(fmt::to_string(rid));
					str.push_back(')');
				}
			}
		}
		else
		{
			str = "RID ";
			str.append(fmt::to_string(rid));
		}
		return str;
	}

	void ScAccount::deinit()
	{
		sc_accounts.clear();
		request_queue = {};
		sc_account_callbacks.clear();
	}
}
