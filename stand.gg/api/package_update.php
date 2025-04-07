<?php
http_response_code(400);
exit;

if(empty($_POST["account_id"])||empty($_POST["name"])||empty($_POST["version"])||empty($_POST["download"]))
{
	http_response_code(400);
	exit;
}
require "../src/dbinclude.php";
$download = $_POST["download"];
if(substr($download, 0, 7) == "http://")
{
	$download = substr($download, 7);
}
else if(substr($download, 0, 8) == "https://")
{
	$download = substr($download, 8);
}
$db->query("UPDATE `packages` SET `version`=?, `download`=? WHERE `name`=? AND `owner`=?", "ssss", $_POST["version"], $download, $_POST["name"], $_POST["account_id"]);
