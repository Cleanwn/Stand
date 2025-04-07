<?php
if(empty($_POST["account_id"])||empty($_POST["key"]))
{
	http_response_code(400);
	exit;
}
require "../src/include.php";
$parsed_key = parseKey($_POST["key"]);
if(!isKeyTypeViableForTopup($parsed_key[0]))
{
	http_response_code(400);
	exit;
}
$coins = getKeyCoinsValue($parsed_key);
if($coins == 0)
{
	http_response_code(404);
	exit;
}
$account_res = $db->query("SELECT `coins` FROM `accounts` WHERE `id`=?", "s", $_POST["account_id"]);
if(empty($account_res))
{
	http_response_code(404);
	exit;
}
setKeyUsedForTopup($parsed_key, $_POST["account_id"]);
$account_res[0]["coins"] = intval($account_res[0]["coins"]) + $coins;
$db->query("UPDATE `accounts` SET `coins`=? WHERE `id`=?", "is", $account_res[0]["coins"], $_POST["account_id"]);
header("Content-Type: application/json");
echo json_encode($account_res[0]);
