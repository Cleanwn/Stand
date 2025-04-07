<?php
if (empty($argv))
{
	http_response_code(418);
	exit;
}

require "include.php";

$db->query("DELETE FROM `menus` WHERE `last_heartbeat`<?", "i", time() - (31 * 60));
