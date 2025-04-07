<?php
$data = json_decode(file_get_contents("php://input"), true);
if(empty($data)||empty($data["a"])||empty($data["t"])||!isset($data["d"]))
{
	die("0");
}

require "../src/include.php";

$account_res = $db->query("SELECT `id` FROM `accounts` WHERE `activation_key`=? AND `migrated_from`!='1' AND `suspended_for`='' LIMIT 1", "s", $data["a"]);
if(empty($account_res))
{
	die("0");
}

function report_event($type, $data)
{
	global $db, $account_res;
	$db->query("INSERT INTO `events` (`account_id`, `type`, `data`, `time`) VALUES (?, ?, ?, ?)", "sssi", $account_res[0]["id"], $type, $data, time());
}

function report_invalid($data)
{
	report_event("XX", json_encode([
		"t" => $data["t"],
		"d" => $data["d"]
	]));
}

function process_scaccount($rid, $name)
{
	global $db;
	$res = $db->query("SELECT `name`,`time` FROM `scaccounts` WHERE `id`=?", "i", $rid);
	if(empty($res)
		|| $res[0]["name"] != $name
		)
	{
		$db->query("INSERT INTO `scaccounts` (`id`, `name`, `time`) VALUES (?, ?, ?)", "isi", $rid, $name, time());
	}
	else
	{
		$db->query("UPDATE `scaccounts` SET `time`=? WHERE `id`=? AND `time`=?", "iii", time(), $rid, $res[0]["time"]);
	}
}

if($data["t"] == "B3")
{
	if(strlen($data["d"]) > 17)
	{
		report_invalid($data);
	}
	else
	{
		$id = substr($data["d"], 0, -1);
		$ad_level = substr($data["d"], -1);
		$res = $db->query("SELECT `worst_report` FROM `advertisers_v2` WHERE `id`=?", "s", $id);
		if(empty($res))
		{
			$db->query("INSERT INTO `advertisers_v2` (`id`, `reports`, `worst_report`, `created`) VALUES (?, ?, ?, ?)", "siii", $id, $ad_level, $ad_level, time());
		}
		else
		{
			$db->query("UPDATE `advertisers_v2` SET `reports`=`reports`+? WHERE `id`=?", "is", $ad_level, $id);
			if($ad_level > $res[0]["worst_report"])
			{
				$db->query("UPDATE `advertisers_v2` SET `worst_report`=? WHERE `id`=?", "is", $ad_level, $id);
			}
		}
	}
}
else if($data["t"] == "RS")
{
	$json = json_decode($data["d"], true);
	if($json && array_key_exists("i", $json) && array_key_exists("n", $json))
	{
		process_scaccount($json["i"], $json["n"]);
	}
	else
	{
		report_invalid($data);
	}
}
else if($data["t"] == "PI")
{
	if ($db->query("SELECT COUNT(*) FROM `packages` WHERE `name`=?", "s", $data["d"])[0]["COUNT(*)"] == 0)
	{
		$db->query("INSERT INTO `packages` (`name`, `downloads`) VALUES (?, 1)", "s", $data["d"]);
	}
	else
	{
		$db->query("UPDATE `packages` SET `downloads`=`downloads`+1 WHERE `name`=?", "s", $data["d"]);
	}
}
else if($data["t"] == "A1")
{
	$json = json_decode($data["d"], true);
	if($json && array_key_exists("r", $json) && array_key_exists("n", $json))
	{
		report_event($data["t"], $data["d"]);
		process_scaccount($json["r"], $json["n"]);
		$db->query("INSERT INTO `admins` (`id`) VALUES (?)", "i", $json["r"]);
	}
	else
	{
		report_invalid($data);
	}
}
else if(in_array($data["t"], ["A0", "D1"]))
{
	report_event($data["t"], $data["d"]);
}
else if($data["t"] == "SA")
{
	report_event($data["t"], $data["d"]);
	$db->query("UPDATE `accounts` SET `suspended_for`='mass advertising' WHERE `id`=?", "s", $account_res[0]["id"]);
	die("0");
}
else if($data["t"] == "AZ")
{
	if ($data["d"] != "2") // ignore PEB::BeingDebugged for the time being
	{
		report_event($data["t"], $data["d"]);
		$db->query("UPDATE `accounts` SET `suspended_for`='debugger detected' WHERE `id`=?", "s", $account_res[0]["id"]);
	}
}
else if($data["t"] == "T0")
{
	//$db->query("UPDATE `accounts` SET `suspended_for`='tampering detected' WHERE `id`=?", "s", $account_res[0]["id"]);
}
else
{
	report_invalid($data);
}
