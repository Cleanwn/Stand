<?php
// Check args
if(!empty($_GET["0"]))
{
	// Check license
	require __DIR__."/../common/include.php";
	$res = $db->query("SELECT `privilege` FROM `accounts` WHERE `activation_key`=? AND `suspended_for`=''", "s", $_GET["0"]);
	if(!empty($res))
	{
		echo $res[0]["privilege"];
		exit;
	}
}
echo "0";
