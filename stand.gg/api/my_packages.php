<?php
if(empty($_POST["account_id"]))
{
	http_response_code(400);
	exit;
}
require "../src/dbinclude.php";
header("Content-Type: application/json");
echo json_encode($db->query("SELECT `name`,`version`,`download` FROM `packages` WHERE `owner`=?", "s", $_POST["account_id"]));
