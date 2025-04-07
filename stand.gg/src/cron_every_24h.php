<?php
if (empty($argv))
{
	http_response_code(418);
	exit;
}

require "include.php";

// Advertisers list leniency.
$db->query("DELETE FROM `advertisers_v2` WHERE `reports` <= 40");
$db->query("DELETE FROM `advertisers_v2` WHERE `reports` <= 80 AND `worst_report`=4 AND `created` < ".days_ago(90));
$db->query("DELETE FROM `advertisers_v2` WHERE `reports` <= 80 AND `worst_report`=3 AND `created` < ".days_ago(28));
$db->query("DELETE FROM `advertisers_v2` WHERE `reports` <= 80 AND `worst_report`=2 AND `created` < ".days_ago(21));
$db->query("DELETE FROM `advertisers_v2` WHERE `reports` <= 80 AND `worst_report`=1 AND `created` < ".days_ago(14));

// Detect IP addresses that have logged in to 4 or more accounts, and suspend them all.
foreach($db->query("SELECT `ip`, COUNT(*) FROM `account_devices` WHERE `ip` != 0 GROUP BY `ip` ORDER BY COUNT(*) DESC") as $row)
{
	if ($row["COUNT(*)"] < 4)
	{
		break;
	}
	foreach ($db->query("SELECT `account_id` FROM `account_devices` WHERE `ip`=?", "i", $row["ip"]) as $row2)
	{
		$db->query("UPDATE `accounts` SET `suspended_for`='compromised account (V)' WHERE `id`=? AND `suspended_for`=''", "s", $row2["account_id"]);
	}
}

// Clean up limited-time tracking tables.
$db->query("DELETE FROM `account_devices` WHERE `created`<?", "i", days_ago(7));
$db->query("DELETE FROM `ip_ratelimit`");

// D0: 50 or more regenerates within 24 hours -- to avoid regenerate api abuse
// D1: 5 or more regenerates with a different device than before within 24 hours -- to catch instances where 2 people have been given the same account id, which would not be enough for account sharing detection to trigger
$db->query("UPDATE `accounts` SET `suspended_for`='excessive regenerating (D0)' WHERE `regens` >= 50");
$db->query("UPDATE `accounts` SET `suspended_for`='excessive regenerating (D1)' WHERE `switch_regens` >= 5");
$db->query("UPDATE `accounts` SET `regens`=0, `switch_regens`=0 WHERE `suspended_for`=''");
$db->query("UPDATE `accounts` SET `hwid_changes`=0");

// Delete recently-created accounts that don't seem properly set up
foreach ($db->query("SELECT * FROM `accounts` WHERE `suspended_for`='' AND `last_known_discord_id`='' AND `last_known_identity`='' AND `had_email`='' AND `created_quiz_success`=0 AND `created`>?", "i", days_ago(2)) as $account)
{
	//if ($db->query("SELECT COUNT(*) FROM `account_devices` WHERE `account_id`=?", "s", $account["id"])[0]["COUNT(*)"] == 0)
	{
		if ($db->query("SELECT COUNT(*) FROM `upgrades` WHERE `used_by`=?", "s", $account["id"])[0]["COUNT(*)"] == 0)
		{
			$db->query("DELETE FROM `account_devices` WHERE `account_id`=?", "s", $account["id"]);
			$db->query("DELETE FROM `accounts` WHERE `id`=?", "s", $account["id"]);
			$db->query("UPDATE `license_keys` SET `used_by`='' WHERE `used_by`=?", "s", $account["id"]);
		}
	}
}
