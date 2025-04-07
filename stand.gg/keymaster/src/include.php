<?php
require __DIR__."/../../src/dbinclude.php";

$exposed_acctid_digits = 6;

function isDynamicRequest()
{
	return !empty($_POST["auth"]);
}

function checkAccess(): void
{
	global $db;
	if ($db->query("SELECT COUNT(*) FROM `accounts` WHERE `id`=? AND `keymaster`=1", "s", $_POST["auth"] ?? $_GET["auth"])[0]["COUNT(*)"] != 1)
	{
		throw new Exception("Access denied.");
	}
}

function expandAccountId(string $acctid): string
{
	global $db;
	$res = $db->query("SELECT `id` FROM `accounts` WHERE `id` LIKE ?", "s", $acctid."%");
	if (count($res) > 1)
	{
		throw new Exception("Not enough digits for a unique result.");
	}
	if (count($res) == 1)
	{
		return $res[0]["id"];
	}
	throw new Exception("No matching account found.");
}

function collapseAccountId(string $acctid): string
{
	global $exposed_acctid_digits;
	return substr($acctid, 0, $exposed_acctid_digits);
}

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

function time2html($time): string
{
	return "<span data-time=\"".$time."\"></span>";
}

function acctidToHtml(string $acctid): string
{
	return "<a href=\"./account-lookup#$acctid\">account <code>$acctid</code></a>";
}

function discidToHtml(string $discid): string
{
	return "<a href=\"./discord-lookup#$discid\">$discid</a>";
}

function printAccountInfo(string $acctid): void
{
	global $db;
	$acct = $db->query("SELECT `privilege`, `suspended_for`, `coins`, `last_known_identity`, `had_email`, `created` FROM `accounts` WHERE `id`=?", "s", $acctid)[0];

	echo "<h2 style=\"margin-top:0\">Account <code>".collapseAccountId($acctid)."</code></h2>";
	echo "<ul>";
	if ($acct["created"])
	{
		echo "<li>Created ".time2html($acct["created"])."</li>";
	}
	echo "<li>".privilegeToString($acct["privilege"])."</li>";
	if ($acct["suspended_for"])
	{
		echo "<li>Suspended for ".$acct["suspended_for"];
		if ($acct["suspended_for"] == "mass advertising")
		{
			$res = $db->query("SELECT `data` FROM `events` WHERE `account_id`=? AND `type`='SA'", "s", $acctid);
			if ($res)
			{
				echo "<ul>";
				foreach($res as $row)
				{
					echo "<li>".$row["data"]."</li>";
				}
				echo "</ul>";
			}
		}
		else if (substr($acct["suspended_for"], 0, 22) == "excessive regenerating")
		{
			$res = $db->query("SELECT `regens`, `switch_regens` FROM `accounts` WHERE `id`=?", "s", $acctid)[0];
			echo "<ul>";
			echo "<li>".$res["regens"]." regen</li>";
			echo "<li>".$res["switch_regens"]." device switch + regen</li>";
			echo "</ul>";
		}
		echo "</li>";
	}
	if ($acct["coins"])
	{
		echo "<li>Has balance</li>";
	}
	if (!$acct["last_known_identity"])
	{
		echo "<li>Never used Stand for GTA V</li>";
	}
	if ($acct["had_email"])
	{
		echo "<li>Received account id via email";
		if ($acct["had_email"] != "1")
		{
			echo " <a href=\"./resend?acctid=".collapseAccountId($acctid)."&auth=".$_POST["auth"]."\" target=\"_blank\">Resend</a>";
		}
		echo "</li>";
	}
	echo "</ul>";

	if ($discs = $db->query("SELECT `discord_id`, `last_join` FROM `account_discords` WHERE `account_id`=? ORDER BY `last_join` DESC", "s", $acctid))
	{
		echo "<h3>Linked Discord Accounts</h3>";
		echo "<ul>";
		foreach ($discs as $disc)
		{
			echo "<li>".discidToHtml($disc["discord_id"]);
			if ($disc["last_join"])
			{
				echo ", last joined ".time2html($disc["last_join"]);
			}
			echo "</li>";
		}
		echo "</ul>";
	}
}

function simplifyKey(string $key): string
{
	$key = strtolower($key);
	if(substr($key, 0, 5) == "stand")
	{
		$key = substr($key, 5);
	}
	if(substr($key, 0, 1) == "-")
	{
		$key = substr($key, 1);
	}
	if(substr($key, 0, 8) == "activate")
	{
		$key = substr($key, 8);
	}
	if(substr($key, 0, 1) == "-")
	{
		$key = substr($key, 1);
	}
	return $key;
}

function lookupLicenseKey(string $key): ?array
{
	$key = simplifyKey($key);
	if(strlen($key) == 30)
	{
		global $db;
		$res = $db->query("SELECT `privilege`,`used_by` FROM `license_keys` WHERE `key`=?", "s", $key);
		if(!empty($res))
		{
			return [
				"key" => $key,
				"privilege" => $res[0]["privilege"],
				"used_by" => $res[0]["used_by"],
			];
		}
	}
	else if(strlen($key) == 31)
	{
		global $db;
		$res = $db->query("SELECT `id` FROM `accounts` WHERE `migrated_from`=?", "s", $key);
		if(!empty($res))
		{
			return [
				"key" => $key,
				"used_by" => $res[0]["id"],
			];
		}
		$res = $db->query("SELECT `id`,`activation_key`,`privilege` FROM `accounts` WHERE `migrated_from`='1' AND (`id`=? OR `activation_key`=?)", "ss", $key, $key);
		if(!empty($res))
		{
			return [
				"key" => $res[0]["activation_key"],
				"og_key" => $res[0]["id"],
				"privilege" => $res[0]["privilege"],
				"used_by" => "",
			];
		}
	}
	return null;
}

function printKeyInfo(array $key): void
{
	echo "<h2 style=\"margin-top:0\">Stand-".$key["key"]."</h2>";
	echo "<ul>";
	if (array_key_exists("privilege", $key))
	{
		echo "<li>".privilegeToString($key["privilege"])."</li>";
	}
	if ($key["used_by"])
	{
		echo "<li>Was <b>used</b> to create ".acctidToHtml(collapseAccountId($key["used_by"]))."</li>";
	}
	else
	{
		echo "<li>Was <b>not used</b> to create an account";
		if (array_key_exists("og_key", $key)
			&& $key["og_key"] != $key["key"]
			)
		{
			echo "<ul><li>Original key: Stand-".$key["og_key"]."</li></ul>";
		}
		echo "</li>";
	}
	echo "</ul>";
}

function getAccountsByDiscordUser($id): array
{
	global $db;
	return $db->query("SELECT `account_id`, `last_join` FROM `account_discords` WHERE `discord_id`=? ORDER BY `last_join` DESC", "s", $id);
}

function printDiscordUserInfo(array $arr): void
{
	echo "<h2 style=\"margin-top:0\">".$_POST["discid"]."</h2>";
	echo "<ul>";
	foreach ($arr as $acct)
	{
		echo "<li>".acctidToHtml(collapseAccountId($acct["account_id"]));
		if ($acct["last_join"])
		{
			echo ", last joined ".time2html($acct["last_join"]);
		}
		echo "</li>";
	}
	echo "</ul>";
}

function getAccountIdByActivationKey(string $key): ?string
{
	global $db;
	if ($res = $db->query("SELECT `id` FROM `accounts` WHERE `activation_key`=?", "s", $key))
	{
		return $res[0]["id"];
	}
	return null;
}
