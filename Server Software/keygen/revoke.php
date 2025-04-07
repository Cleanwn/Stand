<?php
$keys = [
];

require "common.php";

// Simplify keys
foreach ($keys as $i => $key)
{
	$keys[$i] = trim($key);
}

// Initialise parameters for omni
$note = NULL;
$basic = 0;
$regular = 0;
$ultimate = 0;
$basic_to_regular = 0;
$regular_to_ultimate = 0;
$basic_to_ultimate = 0;

// Utility functions
function privilegeToString(int $priv): string
{
	switch ($priv)
	{
		case 1: return "Basic";
		case 2: return "Regular";
		case 3: return "Ultimate";
	}
	return "Privilege $priv";
}

function processNote($res)
{
	global $note;
	if($note === NULL)
	{
		$note = $res[0]["note"];
	}
	else if ($note != $res[0]["note"])
	{
		echo "Unexpected note: ".$res[0]["note"].". Skipping.\n";
		return true;
	}
	return false;
}

function collapseAccountId(string $acctid): string
{
	return substr($acctid, 0, 6);
}

// Iterate license keys
foreach ($keys as $key)
{
	if (strlen($key) != 30)
	{
		continue;
	}
	echo "Stand-".$key.": ";
	$res = $db->query("SELECT `privilege`,`note`,`used_by` FROM `license_keys` WHERE `key`=?", "s", $key);
	if(!$res)
	{
		echo "Not found.\n";
		continue;
	}
	echo privilegeToString($res[0]["privilege"]).". ";
	if (processNote($res))
	{
		continue;
	}
	if($account_id = $res[0]["used_by"])
	{
		echo "Used to create account ".collapseAccountId($account_id).". ";
		if ($suspended_for = $db->query("SELECT `suspended_for` FROM `accounts` WHERE `id`=?", "s", $account_id)[0]["suspended_for"])
		{
			echo "Suspended for $suspended_for. ";
			if ($suspended_for != "compromised account" && $suspended_for != "compromised account (T)")
			{
				echo "Skipping.\n";
				continue;
			}
		}
		$db->query("UPDATE `accounts` SET `suspended_for`='reseller revoke' WHERE `id`=?", "s", $account_id);
		echo "Revoked.\n";
		//$db->query("DELETE FROM `account_devices` WHERE `account_id`=?", "s", $account_id);
		//$db->query("DELETE FROM `account_discords` WHERE `account_id`=?", "s", $account_id);
		//$db->query("DELETE FROM `menus` WHERE `account_id`=?", "s", $account_id);
		//$db->query("DELETE FROM `accounts` WHERE `id`=?", "s", $account_id);
	}
	else
	{
		$db->query("DELETE FROM `license_keys` WHERE `key`=?", "s", $key);
		echo "Unused. Yeeted.\n";
	}

	switch($res[0]["privilege"])
	{
		case 1: ++$basic; break;
		case 2: ++$regular; break;
		case 3: ++$ultimate; break;
	}
}

// Iterate upgrade keys
foreach ($keys as $key)
{
	if (strlen($key) != 31)
	{
		continue;
	}
	echo "Stand-Upgrade-".$key.": ";
	$res = $db->query("SELECT `used_by`,`privilege_from`,`privilege_to`,`note` FROM `upgrades` WHERE `key`=?", "s", $key);
	if(!$res)
	{
		echo "Not found.\n";
		continue;
	}
	$typename = privilegeToString($res[0]["privilege_from"])." to ".privilegeToString($res[0]["privilege_to"]);
	echo $typename.". ";
	if (processNote($res))
	{
		continue;
	}
	if($account_id = $res[0]["used_by"])
	{
		echo "Used by account ".collapseAccountId($account_id).". ";
		if ($acct = $db->query("SELECT `privilege` FROM `accounts` WHERE `id`=?", "s", $account_id)[0])
		{
			$acctpriv = $acct["privilege"];
			if ($acctpriv != $res[0]["privilege_to"])
			{
				echo "Account is ".privilegeToString($acctpriv).". Skipping.\n";
				continue;
			}
			$db->query("UPDATE `accounts` SET `privilege`=? WHERE `id`=?", "is", $res[0]["privilege_from"], $account_id);
			echo "Downgraded.\n";
		}
	}
	else
	{
		echo "Unused.\n";
	}
	$db->query("DELETE FROM `upgrades` WHERE `key`=?", "s", $key);

	switch ($typename)
	{
		case "Basic to Regular": ++$basic_to_regular; break;
		case "Regular to Ultimate": ++$regular_to_ultimate; break;
		case "Basic to Ultimate": ++$basic_to_ultimate; break;
	}
}

if($note !== NULL)
{
	echo "Note: ".$note."\n";
	passthru("php omni.php \"$note\" $basic $regular $ultimate $basic_to_regular $regular_to_ultimate $basic_to_ultimate");
}
