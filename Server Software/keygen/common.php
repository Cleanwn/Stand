<?php
require "vendor/autoload.php";

$db = new hellsh\smw(
	"localhost",
	"web",
	"zP7gBygUPacLMnzN3yGmFhmMXazMgccZDn5eRFNfXPqXge4xPJHxAEN9QUzPnZkFNAnxk8fp7g8RfxE3SN66UJV3zm7f3eMPCXJgRaqkA5tVQErSi373WFBvEGUuweGN",
	"stand"
);

function generateKey($len) : string
{
	$str = "";
	for($i = 0; $i < $len; $i++)
	{
		$c = rand(0, 35);
		if($c > 9)
		{
			$c += 39;
		}
		$str .= chr(48 + $c);
	}
	return $str;
}
