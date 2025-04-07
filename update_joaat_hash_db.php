<?php
$data_dir = __DIR__."/../gta-v-joaat-hash-db/out/";

function consumeDb(string $db, array $exclude = [], $hash = false)
{
	global $data_dir, $cpp_fp, $hpp_fp;
	$lines = explode("\n", file_get_contents($data_dir.$db."-dec_signed.csv"));
	array_shift($lines);
	$objects = [];
	foreach($lines as $line)
	{
		if($line=trim($line))
		{
			$arr = explode(",", $line);
			if($arr[1])
			{
				if(!in_array($arr[1], $exclude))
				{
					array_push($objects, $arr[1]);
				}
			}
		}
	}
	sort($objects);
	$count = 0;
	if($hash)
	{
		foreach($objects as $object)
		{
			++$count;
			fwrite($cpp_fp, "\t\tATSTRINGHASH(\"".$object."\"),\r\n");
		}
	}
	else
	{
		foreach($objects as $object)
		{
			++$count;
			fwrite($cpp_fp, "\t\t\"".$object."\",\r\n");
		}
		fwrite($hpp_fp, $count."]");
	}
	fwrite($hpp_fp, ";");
	return $objects;
}

$cpp_fp = fopen("Stand/joaat_hash_db.cpp", "w");
$hpp_fp = fopen("Stand/joaat_hash_db.hpp", "w");
fwrite($cpp_fp, "#include \"joaat_hash_db.hpp\"\r\n\r\n#include \"atStringHash.hpp\"\r\n\r\nnamespace Stand\r\n{\r\n");
fwrite($hpp_fp, "#pragma once\r\n\r\n#include \"hashtype.hpp\"\r\n\r\nnamespace Stand\r\n{\r\n");

fwrite($cpp_fp, "\tconst char* g_objects_downtown[] = {\r\n");
fwrite($hpp_fp, "\textern const char* g_objects_downtown[");
$exclude=consumeDb("objects_downtown");
fwrite($cpp_fp, "\t};\r\n");
fwrite($hpp_fp, "\r\n");

fwrite($cpp_fp, "\tconst char* g_objects[] = {\r\n");
fwrite($hpp_fp, "\textern const char* g_objects[");
consumeDb("objects_complete",$exclude);
fwrite($cpp_fp, "\t};\r\n");
fwrite($hpp_fp, "\r\n");

fwrite($cpp_fp, "\tstd::unordered_set<hash_t> g_objects_crash = {\r\n");
fwrite($hpp_fp, "\textern std::unordered_set<hash_t> g_objects_crash");
consumeDb("objects_crash",[],true);
fwrite($cpp_fp, "\t};\r\n");
fwrite($hpp_fp, "\r\n");

fwrite($cpp_fp, "}\r\n");
fwrite($hpp_fp, "}\r\n");
fclose($cpp_fp);
fclose($hpp_fp);
