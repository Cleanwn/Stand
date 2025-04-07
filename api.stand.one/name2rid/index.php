<?php
exit; // retired for now, also would have to ask wanghzyzhh@gmail.com for IP whitelisting.

// Check args
if(empty($_GET["0"])||empty($_GET["1"]))
{
	die("0");
}

// Check license
require __DIR__."/../common/include.php";
$res = $db->query("SELECT `privilege` FROM `licenses` WHERE `license`=? AND `privilege`>=2", "s", $_GET["0"]);
if(count($res) == 0)
{
	die("0");
}

// Increment request count
$db->query("UPDATE `licenses` SET `name2rid`=`name2rid` + 1 WHERE `license`=? LIMIT 1", "s", $_GET["0"]);

// Close db con
$db->close();

// Forward request
$res = file_get_contents("https://api.julym.com/gta/?name=".urlencode($_GET["1"]));
$json = json_decode($res, true);
if(!$json||!is_numeric(@$json["id"]))
{
	die("0");
}
die($json["id"]);
