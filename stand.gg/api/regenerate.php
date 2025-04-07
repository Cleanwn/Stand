<?php
if(empty($_POST["account_id"])||strlen($_POST["account_id"])!=31)
{
	http_response_code(400);
	exit;
}
require "../src/include.php";
header("Content-Type: text/plain");
$key=generateKey(31);
$res = $db->query("SELECT `activation_key`, `last_regen_ip`, `last_regen_ua_hash`, `last_regen_time` FROM `accounts` WHERE `id`=? AND `migrated_from`!='1' AND `suspended_for`=''", "s", $_POST["account_id"]);
if (count($res) == 1)
{
	require "../src/antisharing.php";
	$allow_regen = false;
	$ip = antisharing_getIp();
	$ua_hash = antisharing_getUaHash();
	if ($ip != $res[0]["last_regen_ip"] || $ua_hash != $res[0]["last_regen_ua_hash"])
	{
		if ($ip != $res[0]["last_regen_ip"] && $ua_hash != $res[0]["last_regen_ua_hash"])
		{
			$db->query("UPDATE `accounts` SET `switch_regens`=`switch_regens`+1 WHERE `id`=?", "s", $_POST["account_id"]);
		}
		$db->query("UPDATE `accounts` SET `last_regen_ip`=?, `last_regen_ua_hash`=? WHERE `id`=?", "iis", $ip, $ua_hash, $_POST["account_id"]);
		$allow_regen = true;
	}
	else
	{
		$allow_regen = (time() - $res[0]["last_regen_time"]) > (60 * 60 * 24);
	}
	if ($allow_regen)
	{
		$db->query("UPDATE `accounts` SET `activation_key`=?, `regens`=`regens`+1, `last_regen_time`=? WHERE `id`=?", "sis", $key, time(), $_POST["account_id"]);
		$db->query("DELETE FROM `menus` WHERE `account_id`=?", "s", $_POST["account_id"]);
		pubSubSend($res[0]["activation_key"], "c");
	}
	else
	{
		$db->query("UPDATE `accounts` SET `regens`=`regens`+1 WHERE `id`=?", "s", $_POST["account_id"]);
		$key = $res[0]["activation_key"];
	}
}
else
{
	// Don't want to reveal if an account id is valid while bypassing antisharing_tallyAccountAccess
}
echo $key;
