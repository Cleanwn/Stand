<?php
// Adapted from https://github.com/calamity-inc/gta-v-joaat-hash-db/blob/senpai/extract_stats.php
// Before running: Search OpenIV for "statssetup" and extract all results into the same folder as this script.

$fh = fopen("raw/stats.txt", "w");
$processed_stats = array(); // Some stats duplicate in OpenIV. This is very sad.
fwrite($fh, "    static inline Stat stats[] = {\n");

function getAgnosticField($stat, $field, $can_be_empty = false)
{
	$res = $stat[$field];
	if (empty($res))
	{
		$res = $stat[strtolower($field)];
		if (empty($res) && !$can_be_empty)
		{
			echo "getAgnosticField failed on:\n";
			var_dump($stat);
			exit;
		}
	}
	return $res;
}

function formatStat($stat, $name)
{
	global $stattypes;
	$name = '      Stat("' . $name . '"';
	$type = getAgnosticField($stat, "Type");
	if (!empty($type))
	{
		$field = getAgnosticField($stat, "Packed", true);
		if (!empty($field))
		{
			$name .= ", true, " . strtoupper($field);
			$name = "// ".$name;
		}
		else
		{
			$name .= ", false, " . strtoupper($type);
		}
	}
	else
	{
		echo "Unexpected absent type on stat: " . $name . "\n";
		var_dump($name);
		exit;
	}
	return $name . "),";
}

function markStatAsProcessed($name)
{
	global $processed_stats;
	$processed_stats[trim($name)] = true;
}

function wasStatProcessed($name)
{
	global $processed_stats;
	return array_key_exists(trim($name), $processed_stats);
}

function processStatName($name)
{
	if (wasStatProcessed($name))
	{
		return false;
	}

	markStatAsProcessed($name);
	return true;
}

function processCharStat($mp, $stat)
{
	global $fh;
	$arr = $mp ? ["MP0_", "MP1_"] : ["SP0_", "SP1_", "SP2_"];
	foreach($arr as $prefix)
	{
		$name = $prefix . getAgnosticField($stat, "Name");
		if (processStatName($name))
		{
			$line = formatStat($stat, $name);
			fwrite($fh, $line."\n");
		}
	}
}

function processStats($mp, $stats)
{
	global $fh;
	foreach($stats->stat as $stat)
	{
		$name = getAgnosticField($stat, "Name");
		if($stat["characterStat"] == "true")
		{
			processCharStat($mp, $stat);
		}
		else if (processStatName($name))
		{
			$name = formatStat($stat, $name);
			fwrite($fh, $name."\n");
		}
	}
}

foreach(scandir(".") as $file)
{
	if(substr($file, -4) == ".xml")
	{
		$mp = substr($file, 0, 2) == "mp";
		$stats = simplexml_load_file($file)->stats;
		if($stats->Category)
		{
			foreach($stats->Category as $category)
			{
				processStats($mp, $category);
			}
		}
		else
		{
			processStats($mp, $stats);
		}
	}
}
echo "Success.\n";

fwrite($fh, "};");
fclose($fh);