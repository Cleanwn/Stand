<?php
$activation_key = file_get_contents("php://input");
if(strlen($activation_key) != 31)
{
	exit;
}
require "../src/include.php";
$account_res = $db->query("SELECT `id` FROM `accounts` WHERE `activation_key`=? AND `migrated_from`!='1' LIMIT 1", "s", $activation_key);
if(!empty($account_res))
{
	$db->query("DELETE FROM `menus` WHERE `account_id`=?", "s", $account_res[0]["id"]);
}
