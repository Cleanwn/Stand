<?php
if(empty($_POST["account_id"])||strlen($_POST["account_id"])!=31)
{
	http_response_code(400);
	exit;
}
require "../src/include.php";
require "../src/antisharing.php";
$res = $db->query("SELECT COUNT(*) FROM `accounts` WHERE `id`=? AND `migrated_from`!='1' AND `suspended_for`=''", "s", $_POST["account_id"]);
if($res[0]["COUNT(*)"] != 1)
{
	http_response_code(400);
	exit;
}
if (!antisharing_tallyAccountAccess($_POST["account_id"]))
{
	http_response_code(400);
	exit;
}
$lt=generateKey(16);
$db->query("UPDATE `accounts` SET `link_token`=? WHERE `id`=?", "ss", $lt, $_POST["account_id"]);
header("Content-Type: text/plain");
echo $lt;
