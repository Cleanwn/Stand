<?php
$data = [];
if(!empty($_GET["0"]))
{
	require __DIR__."/../common/include.php";
	$data = $db->query("SELECT `dev`, `pinkeyed` FROM `accounts` WHERE `activation_key`=? AND `suspended_for`=''", "s", $_GET["0"])[0];
	$data["dev"] = (bool)$data["dev"];
	$data["pinkeyed"] = (bool)$data["pinkeyed"];
}
if (empty($data))
{
	$data = [ "dev" => false, "pinkeyed" => false ];
}
header("Content-Type: application/json");
echo json_encode($data);
