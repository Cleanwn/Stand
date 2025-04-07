<?php
function hex2signed($hex)
{
	return unpack("la", pack("l", hexdec($hex)))["a"];
}

function joaat_hex($key)
{
	return hash("joaat", strtolower($key));
}

function joaat_signed($key)
{
	return hex2signed(joaat_hex($key));
}

$fh = fopen("mp_event_enums.csv", "w");
fwrite($fh, "Key, 11, 12, 13, 14, 15\n");
foreach(explode("\n", file_get_contents("data.txt")) as $line)
{
	$line = trim($line);
	fwrite($fh, $line);
	for ($i = 11; $i <= 15; ++$i)
	{
		fwrite($fh, ", ");
		fwrite($fh, joaat_signed("Globals.MP_Event_Enums".$i.".".$line));
	}
	fwrite($fh, "\n");
}