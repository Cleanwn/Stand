<?php
if(empty($_POST["account_id"])||empty($_POST["email"])||strlen($_POST["email"])>128||strlen($_POST["account_id"])!=31)
{
	http_response_code(400);
	exit;
}

require "../src/include.php";
$res = $db->query("SELECT `had_email` FROM `accounts` WHERE `id`=?", "s", $_POST["account_id"]);
if(count($res)!=1)
{
	http_response_code(404);
	exit;
}

if($res[0]["had_email"])
{
	http_response_code(418);
	exit;
}

$db->query("UPDATE `accounts` SET `had_email`=? WHERE `id`=?", "ss", $_POST["email"], $_POST["account_id"]);

$ch = curl_init();
curl_setopt_array($ch, [
	CURLOPT_RETURNTRANSFER => true,
	CURLOPT_URL => "http://198.251.88.12/send-mail.php",
	CURLOPT_POST => true,
	CURLOPT_POSTFIELDS => [
		"i" => $_POST["account_id"],
		"r" => $_POST["email"]
	]
]);
echo curl_exec($ch);
curl_close($ch);
