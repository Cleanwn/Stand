<?php
require "common.php";

echo "--- Regular ---\n\n";

for($i = 0; $i < $argv[2]; $i++)
{
	$key = generateKey(30);
	$db->query("INSERT INTO `license_keys` (`key`, `privilege`, `note`) VALUES (?, 2, ?)", "ss", $key, $argv[1]);
	echo "Stand-$key\n";
}

echo "\n";
