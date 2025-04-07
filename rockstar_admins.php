<?php
$fh = fopen("Stand/rockstar_admins.hpp", "w");
fwrite($fh, <<<EOC
#pragma once

#include <cstdint>
#include <string>
#include <utility>

namespace Stand
{
	inline const std::pair<int64_t, const char*> rockstar_admins[] = {

EOC);
foreach(array_reverse(json_decode(file_get_contents("https://stand.gg/api/admins.json"), true)) as $admin)
{
	fwrite($fh, "\t\t{".$admin["id"].", \"".$admin["name"]."\"},\n");
}
fwrite($fh, <<<EOC
	};

	[[nodiscard]] inline bool is_admin_rid(int64_t rid)
	{
		for (const auto& rockstar_admin : rockstar_admins)
		{
			if (rockstar_admin.first == rid)
			{
				return true;
			}
		}
		return false;
	}
}

EOC);
