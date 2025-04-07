#include "yeet_flow.hpp"

#include <soup/ObfusString.hpp>

#include "Auth.hpp"
#include "ConsoleLogger.hpp"
#include "HttpRequestBuilder.hpp"
#include "NetInterface.hpp"
#include "Script.hpp"
#include "Util.hpp"

namespace Stand
{
	static void yeet_job() // OBFUS!
	{
		while (g_auth.sent_next_heartbeat)
		{
			Script::current()->yield();
		}
		HttpRequestBuilder b(HttpRequestBuilder::POST, soup::ObfusString("stand.sh"), soup::ObfusString("/api/logout"));
		b.setPayload(g_auth.activation_key_to_try);
		b.setResponseCallback([](soup::HttpResponse&&)
		{
			yeet_now();
		});
		b.setFailCallback([](std::string&&)
		{
			yeet_now();
		});
		b.dispatch();
	}

	void yeet_flow(ThreadContext thread_context)
	{
		if (g_auth.hasApiCredentials())
		{
			Util::ensureYieldableScriptThread(thread_context, &yeet_job);
		}
		else
		{
			yeet_now();
		}
	}

	void yeet_now() // OBFUS!
	{
		g_console.deinit();

		// Exiting means dtoring everything, and if this is called from an HTTP callback, that includes the thread that is running this code.
		// However, dtoring the thread would mean awaiting its completion. A thread waiting for itself... yeah, so: just invalidate it.
		g_net_interface.thrd.handle = INVALID_HANDLE_VALUE;

		Exceptional::disableExceptionHandling();
		SOUP_IF_UNLIKELY (g_yeeting_with_abort)
		{
			abort();
		}
		TerminateProcess(GetCurrentProcess(), 0);
	}
}
