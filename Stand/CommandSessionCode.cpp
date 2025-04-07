#include "CommandSessionCode.hpp"

#include <soup/ObfusString.hpp>

#include "gsType.hpp"
#include "HttpRequestBuilder.hpp"
#include "is_session.hpp"
#include "natives.hpp"
#include "session_code_cache.hpp"
#include "SessionSpoofing.hpp"

#include "CommandSessionCodeView.hpp"
#include "CommandSessionCodeJoin.hpp"
#include "CommandSessionCodeSpectate.hpp"
#include "CommandSessionCodeLink.hpp"

namespace Stand
{
	static std::string requesting_session_code_for{};

	CommandSessionCode::CommandSessionCode(CommandList* parent)
		: CommandList(parent, LOC("SESSCODE"), CMDNAMES_OBF("sessioncode"))
	{
		this->createChild<CommandSessionCodeView>();
		this->createChild<CommandSessionCodeJoin>();
#if CAN_SPECTATE
		this->createChild<CommandSessionCodeSpectate>();
#endif
		this->createChild<CommandSessionCodeLink>();
	}

	std::string CommandSessionCode::getGsinfo()
	{
		if (is_session_started()
			&& !is_session_transition_active(true, false)
			)
		{
			return SessionSpoofing::getRealSessionInfo().toGsinfo();
		}
		return {};
	}

	bool CommandSessionCode::hasSessionCode()
	{
		return hasSessionCode(getGsinfo());
	}

	bool CommandSessionCode::hasSessionCode(const std::string& gsinfo)
	{
		return session_code_cache.find(gsinfo) != session_code_cache.end();
	}

	std::optional<std::string> CommandSessionCode::getSessionCode()
	{
		auto entry = session_code_cache.find(getGsinfo());
		if (entry != session_code_cache.end())
		{
			return entry->second;
		}
		return std::nullopt;
	}

	std::optional<std::string> CommandSessionCode::getSessionCodeOrStartRequest(const std::string& gsinfo)
	{
		auto entry = session_code_cache.find(gsinfo);
		if (entry != session_code_cache.end())
		{
			return entry->second;
		}
		if (requesting_session_code_for.empty())
		{
			requesting_session_code_for = gsinfo;
			auto b = HttpRequestBuilder(HttpRequestBuilder::POST, soup::ObfusString("stand.sh"), soup::ObfusString("/api/session_code_create"));
			b.setPayload(requesting_session_code_for);
			b.setResponseCallback([](soup::HttpResponse&& resp)
			{
				session_code_cache.emplace(std::move(requesting_session_code_for), std::move(resp.body));
				requesting_session_code_for.clear();
			})
			.setFailCallback([](std::string&&)
			{
				requesting_session_code_for.clear();
			})
			.dispatch();
		}
		return std::nullopt;
	}

	std::optional<std::string> CommandSessionCode::getSessionCodeOrStartRequest(bool& applicable)
	{
		auto gsinfo = getGsinfo();
		if (!gsinfo.empty()
			&& isSessionTypeViable(SessionSpoofing::getRealSessionType())
			)
		{
			applicable = true;
			return getSessionCodeOrStartRequest(gsinfo);
		}
		applicable = false;
		return std::nullopt;
	}

	std::string CommandSessionCode::getSessionCodeForUser()
	{
		auto gsinfo = getGsinfo();
		if (!gsinfo.empty()
			&& isSessionTypeViable(SessionSpoofing::getRealSessionType())
			)
		{
			if (auto code = getSessionCodeOrStartRequest(gsinfo))
			{
				return *code;
			}
			return LANG_GET("GENWAIT");
		}
		return LANG_GET("NA");
	}

	std::string CommandSessionCode::getSessionCodeForUser(int64_t gstype, const std::string& gsinfo)
	{
		if (!gsinfo.empty()
			&& isSessionTypeViable(gstype)
			)
		{
			if (auto code = getSessionCodeOrStartRequest(gsinfo))
			{
				return *code;
			}
			return LANG_GET("GENWAIT");
		}
		return LANG_GET("NA");
	}

	bool CommandSessionCode::isSessionTypeViable(int64_t gstype) noexcept
	{
		return gstype == GS_PUBLIC
			//|| gstype == GS_INVITE
			|| gstype == GS_CLOSED_CREW
			|| gstype == GS_CLOSED_FRIEND
			|| gstype == GS_CREW
			;
	}
}
