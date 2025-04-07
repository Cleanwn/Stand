<?php
$data = file_get_contents("php://input");
require "../src/include.php";
if(strlen($data) != 168)
{
	exit;
}
$res = $db->query("SELECT `code` FROM `session_codes` WHERE `data`=?", "s", $data);
if(!empty($res))
{
	die($res[0]["code"]);
}
$db->query("DELETE FROM `session_codes` WHERE `created`<?", "i", time() - (24 * 60 * 60));
do
{
	$code = strtoupper(generateKey(4));
} while (strpos($code, "O") !== false
	|| strpos($code, "0") !== false
	|| $db->query("SELECT COUNT(*) FROM `session_codes` WHERE `code`=?", "s", $code)[0]["COUNT(*)"] != 0
);
$db->query("INSERT INTO `session_codes` (`code`, `data`, `created`) VALUES (?, ?, ?)", "ssi", $code, $data, time());
echo $code;
