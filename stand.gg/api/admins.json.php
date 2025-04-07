<?php
require "../src/dbinclude.php";

header("Content-Type: application/json");
header("Cache-Control: public, max-age=7200");

$res = $db->query("SELECT `id`,`risk` FROM `admins`");
foreach($res as &$row)
{
	$row["id"] = (int)$row["id"];
	$row["risk"] = (int)$row["risk"];
	$row["name"] = $db->query("SELECT `name` FROM `scaccounts` WHERE `id`=? ORDER BY `time` DESC", "i", $row["id"])[0]["name"];
}
echo json_encode($res);
