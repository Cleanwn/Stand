<?php
if (empty($argv[1]))
{
	die("Arguments: php {$argv[0]} <revision>\n");
}
$revision = $argv[1];

$natives = json_decode(file_get_contents("https://raw.githubusercontent.com/alloc8or/gta5-nativedb-data/master/natives.json"), true);
$common_map = [
	"int" => "int",
	"Hash" => "int",
	"Ped" => "int",
	"Any" => "int",
	"Entity" => "int",
	"Vehicle" => "int",
	"Object" => "int",
	"Cam" => "int",
	"ScrHandle" => "int",
	"FireId" => "int",
	"Blip" => "int",
	"Pickup" => "int",
	"Player" => "int",
	"Interior" => "int",
	"BOOL" => "bool",
	"float" => "float",
	"const char*" => "string",
	"Vector3" => "vector3",

	"Vector3*" => "pointer",
	"Any*" => "pointer",
	"float*" => "pointer",
	"int*" => "pointer",
	"BOOL*" => "pointer",
	"ScrHandle*" => "pointer",
	"Entity*" => "pointer",
	"Ped*" => "pointer",
	"Vehicle*" => "pointer",
	"Object*" => "pointer",
	"Hash*" => "pointer",
	"Blip*" => "pointer",
	"char*" => "pointer",
];
$param_map = $common_map;
$return_map = $common_map + [
	"void" => "void",
];
$unknown_param_types = [];
$unknown_return_types = [];
mkdir(getenv("APPDATA")."\\Stand\\Lua Scripts\\lib\\natives-".$revision);
$fh = fopen(getenv("APPDATA")."\\Stand\\Lua Scripts\\lib\\natives-".$revision."\\init.lua", "w");
$fh_g = fopen(getenv("APPDATA")."\\Stand\\Lua Scripts\\lib\\natives-".$revision."\\g.lua", "w");

function getTypeDeclaration($map, $type)
{
	$str = "--[[";
	if($type == "const char*")
	{
		$str .= $map[$type];	
	}
	else
	{
		$str .= $type;
		if($type != $map[$type])
		{
			$str .= " (".$map[$type].")";
		}
	}
	return $str."]] ";
}

function getNote($suffix = "")
{
	global $revision;
	$version = $revision.$suffix;
	return "-- To require this file, use util.require_natives(\"$version\")\n-- Note that any given version of the natives library may not be compatible with any given script.\n\n";
}

fwrite($fh, getNote());
fwrite($fh_g, getNote(".g"));

function writeNatives($fh, bool $g)
{
	global $natives, $param_map, $return_map, $unknown_param_types, $unknown_return_types;

	foreach($natives as $namespace => $functions)
	{
		if (!$g)
		{
			fwrite($fh, "$namespace={\n");
		}
		else
		{
			fwrite($fh, "-- $namespace\n");
		}
		foreach($functions as $hash => $function)
		{
			foreach($function["params"] as $param)
			{
				if(!array_key_exists($param["type"], $param_map))
				{
					$unknown_param_types[$param["type"]] = $param["type"];
					continue 2;
				}
			}
			if(!array_key_exists($function["return_type"], $return_map))
			{
				$unknown_return_types[$function["return_type"]] = $function["return_type"];
				continue;
			}
			if($function["comment"] != "")
			{
				foreach(explode("\n", $function["comment"]) as $line)
				{
					fwrite($fh, "\t-- $line\n");
				}
			}
			fwrite($fh, "\t");
			fwrite($fh, $function["name"]);
			fwrite($fh, "=".getTypeDeclaration($return_map, $function["return_type"])."function(");
			$first_arg = true;
			foreach($function["params"] as $i => $param)
			{
				if(in_array($param["name"], ["end", "repeat"]))
				{
					$function["params"][$i]["name"] = "_".$param["name"];
				}
				if($first_arg)
				{
					$first_arg = false;
				}
				else
				{
					fwrite($fh, ",");
				}
				fwrite($fh, getTypeDeclaration($param_map, $param["type"]));
				fwrite($fh, $function["params"][$i]["name"]);
			}
			fwrite($fh, ")return native_invoker.unified_".$return_map[$function["return_type"]]."(");
			$types = "";
			foreach($function["params"] as $param)
			{
				fwrite($fh, $param["name"].", ");
				$types .= substr($param_map[$param["type"]], 0, 1);
			}
			fwrite($fh, $hash.", \"".$types."\")end");
			if (!$g)
			{
				fwrite($fh, ",");
			}
			fwrite($fh, "\n");
		}
		if (!$g)
		{
			fwrite($fh, "}\n");
		}
	}
}
writeNatives($fh, false);
writeNatives($fh_g, true);

if($unknown_param_types)
{
	echo "Unknown param types: ".join(", ", $unknown_param_types)."\n";
}
if($unknown_return_types)
{
	echo "Unknown return types: ".join(", ", $unknown_return_types)."\n";
}
