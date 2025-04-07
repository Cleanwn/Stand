<?php
if(isset($_GET["code"]))
{
	require "../src/include.php";
	$res = $db->query("SELECT `data` FROM `session_codes` WHERE `code`=?", "s", $_GET["code"]);
	if(!empty($res))
	{
		die($res[0]["data"]);
	}
}
http_response_code(400);
