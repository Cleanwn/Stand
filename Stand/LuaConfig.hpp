#pragma once

namespace Stand
{
	struct LuaConfig
	{
		inline static CommandToggleNoCorrelation* warn_money;
		inline static CommandToggleNoCorrelation* warn_dangerous;

		inline static CommandListSelect* enforce_silent_start;
		inline static CommandListSelect* enforce_silent_stop;

		inline static CommandToggleNoCorrelation* enable_warnings;
		inline static CommandToggleNoCorrelation* notify_deprecated;
		inline static CommandToggleNoCorrelation* notify_bad_practice;
		inline static CommandToggleNoCorrelation* notify_missed_cleanup_blurrect;
		inline static CommandToggleNoCorrelation* notify_missed_cleanup_arspinner;
		inline static CommandToggleNoCorrelation* notify_missed_cleanup_graceland;
		inline static CommandToggleNoCorrelation* notify_ref_by_path_upgrade;
		inline static CommandToggleNoCorrelation* notify_get_value_fail;
	};
}
