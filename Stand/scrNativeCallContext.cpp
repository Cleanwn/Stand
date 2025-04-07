#include "scrNativeCallContext.hpp"

#include "ida_decl.hpp"

#include "Metrics.hpp"
#include "pointers.hpp"
#include "ScriptMgr.hpp"
#include "vector.hpp"

#if !EXPECT_INVOKE_WONT_FAIL
#define LOG_INVOKE_FAIL false

#if LOG_INVOKE_FAIL
#include "FileLogger.hpp"
#include "Util.hpp"
#endif
#endif

namespace rage
{
	using namespace Stand;

	void scrNativeCallContext::pushArg(const scrVector3& value)
	{
		*reinterpret_cast<scrVector3*>(reinterpret_cast<uint64_t*>(m_args) + m_arg_count) = value;
		m_arg_count += 3;
	}

#if !EXPECT_INVOKE_WONT_FAIL
	bool scrNativeCallContext::canInvoke(::Stand::XoredNativeHash hash)
	{
		return g_script_mgr.entrypoint_map.contains(hash);
	}
#endif

	void scrNativeCallContext::invoke(::Stand::XoredNativeHash x)
	{
		invokePassthrough(x);
		CopyReferencedParametersOut();
		++Metrics::natives_stand_invoked_this_tick;
	}

	void scrNativeCallContext::invokePassthrough(::Stand::XoredNativeHash x)
	{
#if EXPECT_INVOKE_WONT_FAIL
		g_script_mgr.call_map.at(x)(*this);
#else
		if (auto e = g_script_mgr.call_map.find(x); e != g_script_mgr.call_map.end())
		{
			e->second(*this);
		}
#if LOG_INVOKE_FAIL
		else
		{
			g_logger.log(fmt::format("Silently failing call to {}", Util::to_padded_hex_string(x.getHash())));
		}
#endif
#endif
	}

	void scrNativeCallContext::CopyReferencedParametersOut()
	{
		while (buffer_count--)
		{
			orig[buffer_count][0].Float = buffer[buffer_count].x;
			orig[buffer_count][1].Float = buffer[buffer_count].y;
			orig[buffer_count][2].Float = buffer[buffer_count].z;
		}
	}
}
