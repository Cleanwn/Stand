<?php
$native_tables_dir = "..\\GTA-V-Decompiled-Scripts\\native_tables\\";
if(!is_dir($native_tables_dir))
{
	die("$native_tables_dir is not a directory.\n");
}
$extracted_natives = [
	"AAD68E1AB39DA632" => [], // DRAW_DEBUG_SPHERE
	"083A2CA4F2E573BD" => [], // DRAW_DEBUG_BOX
	"3903E216620488E8" => [], // DRAW_DEBUG_TEXT
	"7FDFADE676AA3CB0" => [], // DRAW_DEBUG_LINE
	"1B93E0107865DD40" => [], // SET_CAM_DEBUG_NAME
	//"73B1189623049839" => [], // DRAW_DEBUG_CROSS
	//"175B6BFC15CDD0C5" => [], // SET_DEBUG_LINES_AND_SPHERES_DRAWING_ACTIVE
	//"D8B9A8AC5608FF94" => [], // DRAW_DEBUG_LINE_WITH_TWO_COLOURS
	"A3BB2E9555C05A8F" => [], // DRAW_DEBUG_TEXT_2D
	//"2162C446DFDF38FD" => [], // _LOG_DEBUG_INFO
	"98EFA132A4117BE1" => [], // SET_PED_NAME_DEBUG
	"BFDF984E2C22B94F" => [], // SET_VEHICLE_NAME_DEBUG
	"B736A491E64A32CF" => [], // SET_ENTITY_AS_NO_LONGER_NEEDED
	"2595DD4236549CE3" => [], // SET_PED_AS_NO_LONGER_NEEDED
	"629BFA74418D6239" => [], // SET_VEHICLE_AS_NO_LONGER_NEEDED
	"3AE22DEB5BA5A3E6" => [], // SET_OBJECT_AS_NO_LONGER_NEEDED
];
foreach(scandir($native_tables_dir) as $script_file)
{
	if(substr($script_file, -4) != ".txt")
	{
		continue;
	}
	$script_name = substr($script_file, 0, -4);
	if($script_name == "valentinerpreward2")
	{
		continue;
	}
	$i = 0;
	foreach(file($native_tables_dir.$script_file) as $native)
	{
		$native = rtrim($native);
		if(array_key_exists($native, $extracted_natives))
		{
			$extracted_natives[$native][$script_name] = $i;
		}
		$i++;
	}
}

$fp = fopen("Stand/native_tables.hpp", "w");
fwrite($fp, "#pragma once\r\n\r\nnamespace Stand\r\n{\r\n\tstatic const std::unordered_map<XoredNativeHash, std::unordered_map<hash_t, uint16_t>> native_tables = {\r\n");
foreach($extracted_natives as $native_hash => $entries)
{
	fwrite($fp, "\t\t{ 0x".$native_hash.", {\r\n");
	foreach($entries as $script => $index)
	{
		fwrite($fp, "\t\t\t{ ATSTRINGHASH(\"$script\"), $index },\r\n");
	}
	fwrite($fp, "\t\t}},\r\n");
}
fwrite($fp, "\t};\r\n");
fwrite($fp, "}\r\n");
