<?php
if(empty($_POST["account_id"])||strlen($_POST["account_id"])!=31||!isset($_POST["interested"]))
{
	http_response_code(400);
	exit;
}
require "../src/include.php";
$db->query("UPDATE `accounts` SET `interested`=? WHERE `id`=?", "is", $_POST["interested"], $_POST["account_id"]);
header("Content-Type: application/json");
echo json_encode([
	"interested" => (bool)$_POST["interested"]
]);
