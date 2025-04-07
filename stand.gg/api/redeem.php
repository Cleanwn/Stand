<?php
/*die(json_encode([
	"error" => "Database migration in progress. Please try again in a few seconds."
]));*/

if(empty($_POST["license_key"])||strlen($_POST["license_key"])!=30)
{
	http_response_code(400);
	exit;
}
require "../src/include.php";
$res = $db->query("SELECT `privilege`,`used_by` FROM `license_keys` WHERE `key`=?", "s", $_POST["license_key"]);
header("Content-Type: application/json");
if(empty($res))
{
	die(json_encode([
		"error" => "Invalid license key"
	]));
}
if($res[0]["used_by"])
{
	die(json_encode([
		"error" => "This license key has already been redeemed to create an account"
	]));
}
$account_id = generateKey();
$activation_key = generateKey();
$privilege = (int)$res[0]["privilege"];
$db->query("UPDATE `license_keys` SET `used_by`=? WHERE `key`=?", "ss", $account_id, $_POST["license_key"]);
$db->query("INSERT INTO `accounts` (`id`, `activation_key`, `privilege`, `created`, `dev`, `keymaster`, `created_quiz_success`, `early_toxic`, `pre100`, `used100`, `rndbool`, `interested`, `pinkeyed`) VALUES (?, ?, ?, ?, 0, 0, 0, 0, 0, 0, (floor(rand() * 10) % 2), 0, 0)", "ssii", $account_id, $activation_key, $privilege, time());
die(json_encode([
	"account_id" => $account_id,
	"activation_key" => $activation_key,
	"privilege" => $privilege,
	"created_quiz_success" => false,
]));
