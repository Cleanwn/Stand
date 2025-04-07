<?php
require "src/langwiz.phar";
set_include_path("."); // We just loaded a phar, need to fix include path (https://github.com/php/php-src/issues/10741).
require "src/all.php";

if(!is_dir(__DIR__."/../stand.gg/lang"))
{
	mkdir(__DIR__."/../stand.gg/lang");
}

$format = new Langwiz\AxisFormat();
$langs = $format->getLangsInFolder(".");
$source_code = "en";
if (!array_key_exists($source_code, $langs))
{
	die("Did not find file for source language $source_code\n");
}

$r = $format->getReader();
$w = new Langwiz\SourceToLocalisationWriter($format->getWriter());

echo "Reading {$langs[$source_code]}...";
$src_cont = file_get_contents($langs[$source_code]);
$src = $r->loadSource($src_cont);
echo " Got ".count($src->strings)." strings.\n";

$data = [];

function doPerLanguageTasks(string $code, \Langwiz\LangBase $lang)
{
	$lang->exportWithUntranslated(new Stand\LangCppWriter($code, __DIR__."/../Stand/lang_$code.cpp"));
	$lang->exportWithUntranslated(new Stand\LangJsWriter($code, __DIR__."/../stand.gg/lang/$code.js"));
}

doPerLanguageTasks($source_code, $src);

foreach ($langs as $code => $file)
{
	if ($code == $source_code)
	{
		continue;
	}

	echo "Processing $file...\n";

	$w->loc = $r->loadLocalisation($src, file_get_contents($file));

	$r->read($src_cont, $w);
	file_put_contents($file, $w->w->out);
	$w->w->out = "";

	doPerLanguageTasks($code, $w->loc);

	$data[$code] = [
		"translated" => $w->loc->getTranslatedPercentage()
	];
}

file_put_contents("translations.json", json_encode($data, JSON_PRETTY_PRINT));
