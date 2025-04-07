<?php
$magics = [
	["MAGIC_LABEL", 4],
	["MAGIC_LANG_GET", 4],
	["MAGIC_LANG_GET_W", 4],
	["MAGIC_LANG_GET_EN", 4],
	["MAGIC_CROSSMAP", 8],
	["MAGIC_SCRCMD", 8],
];

function randhex32()
{
	return str_pad(dechex(rand(0, 0xFFFFFFFF)), 8, "0", STR_PAD_LEFT);
}

function randhexbytes($num)
{
	$bytes = randhex32();
	if ($num == 8)
	{
		$bytes .= randhex32();
	}
	return $bytes;
}

$fh = fopen("Stand/xormagics.hpp", "w");
fwrite($fh, "#pragma once\r\n\r\n");
fwrite($fh, "#ifdef STAND_DEBUG\r\n");
foreach ($magics as $magic)
{
	fwrite($fh, "#define ".$magic[0]." 0x".randhexbytes($magic[1])."\r\n");
}
fwrite($fh, "#elif defined(STAND_DEV)\r\n");
foreach ($magics as $magic)
{
	fwrite($fh, "#define ".$magic[0]." 0x".randhexbytes($magic[1])."\r\n");
}
fwrite($fh, "#else\r\n");
foreach ($magics as $magic)
{
	fwrite($fh, "#define ".$magic[0]." 0x".randhexbytes($magic[1])."\r\n");
}
fwrite($fh, "#endif\r\n");
