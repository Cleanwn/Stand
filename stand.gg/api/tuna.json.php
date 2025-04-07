<?php
require "../src/dbinclude.php";

require "data/packages.php";

$blacklist_advertisers_v2 = [];
foreach($db->query("SELECT `id` FROM `advertisers_v2` WHERE `reports` > 40") as $row)
{
	array_push($blacklist_advertisers_v2, $row["id"]);
}

header("Content-Type: application/json");
header("Cache-Control: public, max-age=7200");

function joaat(string $str): int
{
	return hexdec(hash("joaat", $str));
}

echo json_encode([
	"v" => ((filemtime(__DIR__."/bgscript.txt") << 32) ^ filemtime(__DIR__."/blobfish.txt") ^ filemtime(__DIR__."/tuna.json.php")),
	"lnv" => "3274a",
	"cmw" => [
		"75e06c5aa41b329a3d10ea211b97b16268cfdc6e", // luaffi 0.1.1
		"c63b9364fdaf1306eba65293b628797a5c9c7562", // aalib (https://raw.githubusercontent.com/hexarobi/stand-lua-script-repository/main/lib/aalib.dll), needed by some old scripts to play sounds
	],
	"repo" => $packages,
	"b" => file_get_contents("bgscript.txt"),
	"ba" => $blacklist_advertisers_v2,
	"f" => file_get_contents("blobfish.txt"),
	"a" => [
		[joaat("ffi.gta5.special"), [
			"4C 89 4C 24 ? 4C 89 44 24 ? 48 89 54 24 ? 48 89 4C 24 ? 48 83 EC ? 48 C7 44 24 ? ? ? ? ? 48 C7 44 24 ? ? ? ? ? 33 C9 FF 15 ? ? ? ? 48 83 7C 24", // nvCreateProcessA
			"48 89 5C 24 ? 48 89 74 24 ? 48 89 6C 24 ? 48 8D 64 24 ? 48 89 7C 24 ? 48 8D 64 24", // IMachineHashStrategy::ExecCmd
		]],
	],
]);
