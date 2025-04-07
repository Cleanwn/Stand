<?php
$hpp_fh = fopen("Stand/font_nanumgothic.hpp", "w");
fwrite($hpp_fh, "#pragma once\r\n\r\nnamespace Stand\r\n{\r\n\tstruct font_nanumgothic\r\n\t{");
$bin_str = file_get_contents(__DIR__."/Default Theme/NanumGothic.spritefont");
$i = 0;
$chunks = 0;
$buf = "";
function flushBuffer()
{
	global $hpp_fh, $i, $chunks, $buf;
	++$chunks;
	fwrite($hpp_fh, "\r\n\t\tstatic const char chunk_{$chunks}[".strlen($buf)."];");
	$cpp_fh = fopen("Stand/font_nanumgothic_{$chunks}.cpp", "w");
	fwrite($cpp_fh, "#include \"font_nanumgothic.hpp\"\r\n\r\nnamespace Stand\r\n{\n\tconst char font_nanumgothic::chunk_{$chunks}[] = {'\\x".join("', '\\x", array_map("dechex", array_map("ord", str_split($buf))))."' };\r\n}\r\n");
	fclose($cpp_fh);
	$buf = "";
}
foreach(str_split($bin_str) as $byte)
{
	$buf .= $byte;
	if(++$i >= 10000000)
	{
		flushBuffer();
		$i = 0;
	}
}
if($i != 0)
{
	flushBuffer();
}
fwrite($hpp_fh, "\r\n\t};\r\n}\r\n");
fclose($hpp_fh);
