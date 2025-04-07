<?php
if(empty($_POST["account_id"])||strlen($_POST["account_id"])!=31)
{
	http_response_code(400);
	exit;
}
require "../src/include.php";
require "../src/antisharing.php";

// Do IP rate-limiting if this IP is has already logged into the account id to "test".
$ip = antisharing_getIp();
if (empty($db->query("SELECT `ip` FROM `account_devices` WHERE `account_id`=? AND `ip`=?", "si", $_POST["account_id"], $ip)))
{
	$res = $db->query("SELECT `requests` FROM `ip_ratelimit` WHERE `ip`=?", "i", $ip);
	if (empty($res))
	{
		$db->query("INSERT INTO `ip_ratelimit` (`ip`, `requests`) VALUES (?, 1)", "i", $ip);
	}
	else if ($res[0]["requests"] >= 50)
	{
		die("ratelimit");
	}
	else
	{
		$db->query("UPDATE `ip_ratelimit` SET `requests`=`requests`+1 WHERE `ip`=?", "i", $ip);
	}
}

$res = $db->query("SELECT `activation_key`, `privilege`, `suspended_for`, `coins`, `created_quiz_success`, `interested` FROM `accounts` WHERE `id`=? AND `migrated_from`!='1'", "s", $_POST["account_id"]);
if(empty($res))
{
	die("bad");
}
header("Content-Type: application/json");
$res[0]["created_quiz_success"] = (bool)$res[0]["created_quiz_success"];
$res[0]["interested"] = (bool)$res[0]["interested"];
echo json_encode($res[0]);
if($res[0]["suspended_for"] == "")
{
	antisharing_tallyAccountAccess($_POST["account_id"]);
}
