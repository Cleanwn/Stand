<?php
$hpp_fh = fopen("Stand/bin.hpp", "w");
fwrite($hpp_fh, "#pragma once\r\n\r\nnamespace Stand\r\n{\r\n\tstruct bin\r\n\t{");
$cpp_fh = fopen("Stand/bin.cpp", "w");
fwrite($cpp_fh, "#include \"bin.hpp\"\r\n\r\nnamespace Stand\r\n{");
foreach([
	"scripthookv_dll" => __DIR__."/ScriptHookV/bin/ScriptHookV.dll",
	"MakeSpriteFont_exe" => __DIR__."/Default Theme/MakeSpriteFont.exe",
	"textureToggleOff" => __DIR__."/Default Theme/Toggle Off.png",
	"textureToggleOn" => __DIR__."/Default Theme/Toggle On.png",
	"textureToggleOffAuto" => __DIR__."/Default Theme/Toggle Off Auto.png",
	"textureToggleOnAuto" => __DIR__."/Default Theme/Toggle On Auto.png",
	"textureToggleOffList" => __DIR__."/Default Theme/Toggle Off List.png",
	"textureToggleOnList" => __DIR__."/Default Theme/Toggle On List.png",
	"textureList" => __DIR__."/Default Theme/List.png",
	"textureLink" => __DIR__."/Default Theme/Link.png",
	"textureEnabled" => __DIR__."/Default Theme/Enabled.png",
	"textureDisabled" => __DIR__."/Default Theme/Disabled.png",
	"textureUser" => __DIR__."/Default Theme/User.png",
	"textureFriends" => __DIR__."/Default Theme/Friends.png",
	"textureUsers" => __DIR__."/Default Theme/Users.png",
	"textureEdit" => __DIR__."/Default Theme/Edit.png",
	"textureSearch" => __DIR__."/Default Theme/Search.png",
	"textureHeaderLoading" => __DIR__."/Default Theme/Header Loading.png",
	"defaultCustomHeader" => __DIR__."/Default Custom Header.png",

	"textureWantedStar" => __DIR__."/Special Characters/ws.png",
	"textureLock" => __DIR__."/Special Characters/lock.png",
	"textureRs" => __DIR__."/Special Characters/rs.png",
	"textureRsVerified" => __DIR__."/Special Characters/rs-verified.png",
	"textureRsCreated" => __DIR__."/Special Characters/rs-created.png",
	"textureBlankbox" => __DIR__."/Special Characters/blankbox.png",
	"textureNewline" => __DIR__."/Special Characters/newline.png",
	"textureReset" => __DIR__."/Special Characters/reset.png",
	"textureUni0000" => __DIR__."/Special Characters/U+0000.png",
	"textureUni26A0" => __DIR__."/Special Characters/U+26A0.png",
	"textureUni2728" => __DIR__."/Special Characters/U+2728.png",
	"textureUni2764" => __DIR__."/Special Characters/U+2764.png",
	"textureUni1F4AF" => __DIR__."/Special Characters/U+1F4AF.png",
	"textureUni1F60A" => __DIR__."/Special Characters/U+1F60A.png",
	"textureUni1F480" => __DIR__."/Special Characters/U+1F480.png",
	"textureUni1F525" => __DIR__."/Special Characters/U+1F525.png",
	"textureUni1F602" => __DIR__."/Special Characters/U+1F602.png",
	"textureUni1F629" => __DIR__."/Special Characters/U+1F629.png",
	"textureUni1F633" => __DIR__."/Special Characters/U+1F633.png",

	"textureTabSelf" => __DIR__."/Default Theme/Tabs/Self.png",
	"textureTabVehicle" => __DIR__."/Default Theme/Tabs/Vehicle.png",
	"textureTabOnline" => __DIR__."/Default Theme/Tabs/Online.png",
	//"textureTabPlayers" => __DIR__."/Default Theme/Tabs/Players.png", // Same as textureUsers
	"textureTabWorld" => __DIR__."/Default Theme/Tabs/World.png",
	"textureTabGame" => __DIR__."/Default Theme/Tabs/Game.png",
	"textureTabStand" => __DIR__."/Default Theme/Tabs/Stand.png",

	"bgscriptBuiltin" => __DIR__."/stand.gg/api/bgscript.txt",
] as $name => $path)
{
	$bin_str = file_get_contents($path);
	fwrite($hpp_fh, "\r\n\t\tstatic const char {$name}[".strlen($bin_str)."];");
	fwrite($cpp_fh, "\r\n\tconst char bin::{$name}[] = { '\\x".join("', '\\x", array_map("dechex", array_map("ord", str_split($bin_str))))."' };");
}
fwrite($hpp_fh, "\r\n\t};\r\n}\r\n");
fclose($hpp_fh);
fwrite($cpp_fh, "\r\n}\r\n");
fclose($cpp_fh);
