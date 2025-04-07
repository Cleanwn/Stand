<?php
if(!file_exists("vehicles.json"))
{
	file_put_contents("vehicles.json", json_encode(json_decode(file_get_contents("https://github.com/DurtyFree/gta-v-data-dumps/raw/master/vehicles.json"), true), JSON_PRETTY_PRINT));
}

$dlc_names = [
	"TitleUpdate" => "1.01 Game Launch",
	"mpchristmas2017" => "1.42 The Doomsday Heist",
	"mpheist4" => "1.52 Cayo Perico Heist",
	"mpbusiness" => "1.11 Business",
	"mpsmuggler" => "1.41 Smuggler's Run",
	"mpgunrunning" => "1.40 Gunrunning Pack",
	"mpheist3" => "1.49 Diamond Casino Heist",
	"mpbiker" => "1.36 Bikers",
	"mpapartment" => "1.31 Executives and Other Criminals",
	"mpjanuary2016" => "1.32 January 2016",
	"mpheist" => "1.21 Heists DLC",
	"mppilot" => "1.16 SA Flight School",
	"mpexecutive" => "1.34 Further Adventures in Finance and Felony",
	"mpstunt" => "1.35 Cunning Stunts",
	"mpbeach" => "1.06 Beach Bum",
	"mphipster" => "1.14 I'm Not a Hipster",
	"mpimportexport" => "1.37 Import/Export",
	"spupgrade" => "1.18 Next Gen Release",
	"mpbattle" => "1.44 After Hours",
	"mpluxe2" => "1.28 Ill-Gotten Gains Part 2",
	"mpchristmas2018" => "1.46 Arena War",
	"mpvalentines" => "1.10 Valentine's Day Massacre",
	"mphalloween" => "1.30.1 Halloween Surprise",
	"mpvalentines2" => "1.32.1 Be My Valentine",
	"mplowrider" => "1.30 Lowriders",
	"mpassault" => "1.43 Southern SA Super Sport Series",
	"mpvinewood" => "1.48 Diamond Casino & Resort",
	"mpsum" => "1.51 LS Summer Special",
	"mplowrider2" => "1.33 Lowriders: Custom Classics",
	"mpluxe" => "1.27 Ill-Gotten Gains Part 1",
	"mplts" => "1.17 Last Team Standing",
	"mpspecialraces" => "1.38 Special Vehicle Circuit",
	"mpbusiness2" => "1.13 High Life",
	"mpchristmas2" => "1.19 Festive Surprise 2014",
	"mpindependence" => "1.15 Independence Day",
	"mpxmas_604490" => "1.31.1 Festive Surprise 2015",
	"mptuner" => "1.57 LS Tuners",
	"mpsecurity" => "1.58 The Contract",
	"mpg9ec" => "1.61 The Criminal Enterprises",
	"mpsum2" => "1.61 The Criminal Enterprises",
	"mpchristmas3" => "1.64 Drug Wars",
	"mp2023_01" => "1.67 San Andreas Mercenaries",
	"mp2023_02" => "1.68 The Chop Shop",
];

$fh = fopen(__DIR__."/../Stand/vehicle_items.cpp", "w");
fwrite($fh, "#include \"vehicle_items.hpp\"\r\n\r\n#include \"atStringHash.hpp\"\r\n\r\nnamespace Stand\r\n{\r\n\textern const VehicleItem vehicle_items[] = {\r\n");
foreach(json_decode(file_get_contents("vehicles.json"), true) as $vehicle)
{
	$class = strtolower($vehicle["Class"]);
	if($class == "compact")
	{
		$class = "compacts";
	}
	fwrite($fh, "\t\t");
	if ($vehicle["DlcName"] == "mpg9ec")
	{
		fwrite($fh, "//");
	}
	fwrite($fh, "{\"".strtolower($vehicle["Name"])."\", \"".strtolower($vehicle["Manufacturer"])."\", ATSTRINGHASH(\"".$class."\"), \"".$dlc_names[$vehicle["DlcName"]]."\"},\r\n");
}
fwrite($fh, "\t};\r\n}\r\n");
fflush($fh);
fclose($fh);
