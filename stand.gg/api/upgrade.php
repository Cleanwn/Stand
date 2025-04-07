<?php
if(empty($_POST["account_id"])||empty($_POST["upgrade_key"]))
{
	http_response_code(400);
	exit;
}
require "../src/include.php";
header("Content-Type: text/plain");
$license_res = $db->query("SELECT `privilege` FROM `accounts` WHERE `id`=? LIMIT 1", "s", $_POST["account_id"]);
if($license_res)
{
	$upgrade_res = $db->query("SELECT `key`,`privilege_from`,`privilege_to` FROM `upgrades` WHERE `key`=? AND `used_by`='' LIMIT 1", "s", $_POST["upgrade_key"]);
	if(!$upgrade_res)
	{
		die("invalid_or_used");
	}
	if($upgrade_res[0]["privilege_from"] != $license_res[0]["privilege"])
	{
		die("mismatch");
	}

	$db->query("UPDATE `upgrades` SET `used_by`=? WHERE `key`=?", "ss", $_POST["account_id"], $upgrade_res[0]["key"]);
	$db->query("UPDATE `accounts` SET `privilege`=? WHERE `id`=?", "is", $upgrade_res[0]["privilege_to"], $_POST["account_id"]);
	die("upgrade ".$upgrade_res[0]["privilege_to"]);
}
