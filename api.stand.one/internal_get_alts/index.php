<?php
header("Content-Type: application/json");
if(!empty($_GET["0"]))
{
	require __DIR__."/../common/include.php";
	$res = $db->query("SELECT `id` FROM `accounts` WHERE `last_known_discord_id`=? AND `suspended_for`=''", "s", $_GET["0"]);
	if(!empty($res))
	{
		$res = $db->query("SELECT `discord_id`,`last_join` FROM `account_discords` WHERE `account_id`=? AND `discord_id`!=?", "ss", $res[0]["id"], $_GET["0"]);
		echo json_encode($res);
		exit;
	}
}
echo "[]";
