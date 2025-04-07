<?php
$note = $argv[1];

$basic = $argv[2] ?? 0;
$regular = $argv[3] ?? 0;
$ultimate = $argv[4] ?? 0;
$basic_to_regular = $argv[5] ?? 0;
$regular_to_ultimate = $argv[6] ?? 0;
$basic_to_ultimate = $argv[7] ?? 0;

if(!is_dir("out"))
{
	mkdir("out");
}

function generateToFile($what, $amount)
{
	global $note;
	file_put_contents("out/".$what.".txt", shell_exec("php gen_$what.php \"$note\" $amount"));
}

if($basic) generateToFile("basic", $basic);
if($regular) generateToFile("regular", $regular);
if($ultimate) generateToFile("ultimate", $ultimate);
if($basic_to_regular) generateToFile("upgrade_basic_to_regular", $basic_to_regular);
if($regular_to_ultimate) generateToFile("upgrade_regular_to_ultimate", $regular_to_ultimate);
if($basic_to_ultimate) generateToFile("upgrade_basic_to_ultimate", $basic_to_ultimate);
