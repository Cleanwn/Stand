<?php
function get_reason($source)
{
	if ($source == "discord")
	{
		return "account id sharing (D)";
	}
	if ($source == "telegram")
	{
		return "compromised account (T)";
	}
	return NULL;
}

header("Content-Type: application/json");
if(!empty($_GET["0"]) && !empty($_GET["1"]) && get_reason($_GET["1"]))
{
	require __DIR__."/../common/include.php";
	$res = $db->query("SELECT `suspended_for` FROM `accounts` WHERE `id`=?", "s", $_GET["0"]);
	if(!empty($res))
	{
		if (!$res[0]["suspended_for"] || $res[0]["suspended_for"] == "account sharing (B4)" || $res[0]["suspended_for"] == "excessive regenerating (D1)")
		{
			$res = $db->query("UPDATE `accounts` SET `suspended_for`=? WHERE `id`=?", "ss", get_reason($_GET["1"]), $_GET["0"]);
		}
		die("1");
	}
}
echo "0";
