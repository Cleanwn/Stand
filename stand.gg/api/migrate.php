<?php
if(empty($_POST["license_key"])||strlen($_POST["license_key"])!=31)
{
	http_response_code(400);
	exit;
}
require "../src/include.php";
$res = $db->query("SELECT `activation_key`, `privilege`, `coins` FROM `accounts` WHERE `id`=? AND `migrated_from`='1'", "s", $_POST["license_key"]);
header("Content-Type: application/json");
if(empty($res))
{
	$res = $db->query("SELECT `migrated_from` FROM `accounts` WHERE `migrated_from`=?", "s", $_POST["license_key"]);
	if(!empty($res))
	{
		die(json_encode([
			"error" => "License key has already been migrated"
		]));
	}
	die(json_encode([
		"error" => "Invalid license key (must be the original)"
	]));
}
if($res[0]["activation_key"]!=$_POST["license_key"]) // key was regenerated
{
	if(empty($_POST["license_key_2"])) // no second key provided, ask for it
	{
		die(json_encode([
			"error" => "need2"
		]));
	}
	if($_POST["license_key_2"]!=$res[0]["activation_key"]) // second key was provided but wrong
	{
		die(json_encode([
			"error" => "Incorrect current key"
		]));
	}
}
$account_id = generateKey();
$activation_key = generateKey();
$db->query("UPDATE `accounts` SET `id`=?, `activation_key`=?, `migrated_from`=? WHERE `id`=?", "ssss", $account_id, $activation_key, $_POST["license_key"], $_POST["license_key"]);
die(json_encode([
	"account_id" => $account_id,
	"activation_key" => $activation_key,
	"privilege" => (int)$res[0]["privilege"],
	"coins" => (int)$res[0]["coins"],
	"created_quiz_success" => false,
]));
