#pragma once

namespace Stand
{
	struct CleanupHelper
	{
		inline static bool did_non_script_cleanup = false;

		static void execNonScript();
		static void execScript();
		static void execScriptForce();
	};
}
