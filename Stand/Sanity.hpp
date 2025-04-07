#pragma once

namespace Stand
{
	struct Sanity
	{
		inline static bool playerhistory_find_in_non_normal_root_state = false;
		inline static bool playerhistory_insert_in_auth_inconsistent_state = false;
		inline static bool discovered_other_player_without_api_credentials = false;
		inline static bool discovered_other_player_in_invalid_root_state = false;
		inline static bool discovered_other_player_non_free_without_recover_state = false;
	};
}
