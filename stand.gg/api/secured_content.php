<?php
$data = json_decode(file_get_contents("php://input"), true);
if(empty($data)||empty($data["a"])||empty($data["c"]))
{
	exit;
}

if(strpos($data["c"], ".") !== false
	|| substr($data["c"], 0, 4) != "lua/"
	|| strpos(substr($data["c"], 4), "/") !== false
)
{
	exit;
}

require "../src/include.php";

$account_res = $db->query("SELECT `activation_key` FROM `accounts` WHERE `activation_key`=? AND `migrated_from`!='1' AND `suspended_for`='' LIMIT 1", "s", $data["a"]);
if(empty($account_res))
{
	exit;
}

if(!file_exists("/var/www/stand.gg/dl/secured/".$data["c"]))
{
	exit;
}

echo file_get_contents("/var/www/stand.gg/dl/secured/".$data["c"]);
