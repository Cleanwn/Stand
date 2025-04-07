<?php
$joaat_cache = [];

function joaat(string $key): string
{
	global $joaat_cache;
	if (!array_key_exists($key, $joaat_cache))
	{
		$joaat_cache[$key] = strtoupper(hash("joaat", strtolower($key)));
	}
	return $joaat_cache[$key];
}

function escape(string $str): string
{
	return str_replace(["\\", "\""], ["\\\\", "\\\""], $str);
}
