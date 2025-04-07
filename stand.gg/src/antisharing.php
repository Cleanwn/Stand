<?php
function antisharing_getIp()
{
	return ip2long($_SERVER["HTTP_CF_CONNECTING_IP"]);
}

function antisharing_getUaHash()
{
	return crc32($_SERVER["HTTP_USER_AGENT"]);
}

function antisharing_tallyAccountAccess($account_id)
{
	global $db;

	$ip = antisharing_getIp();
	$ua_hash = antisharing_getUaHash();
	$devices = $db->query("SELECT `ip`, `ua_hash` FROM `account_devices` WHERE `account_id`=?", "s", $account_id);
	$is_new_device = true;
	foreach($devices as $device)
	{
		if($device["ip"] == $ip)
		{
			$is_new_device = false;
			if($device["ua_hash"] != $ua_hash)
			{
				$db->query("UPDATE `account_devices` SET `ua_hash`=? WHERE `account_id`=? AND `ip`=?", "isi", $ua_hash, $account_id, $ip);
			}
			break;
		}
		if($device["ua_hash"] == $ua_hash)
		{
			$is_new_device = false;
			if($device["ip"] != $ip)
			{
				$db->query("DELETE FROM `account_devices` WHERE `account_id`=? AND `ip`=?", "si", $account_id, $device["ip"]);
				$db->query("INSERT INTO `account_devices` (`account_id`, `ip`, `ua_hash`, `created`) VALUES (?, ?, ?, ?)", "siii", $account_id, $ip, $ua_hash, time());
			}
			break;
		}
	}
	if($is_new_device)
	{
		$db->query("INSERT INTO `account_devices` (`account_id`, `ip`, `ua_hash`, `created`) VALUES (?, ?, ?, ?)", "siii", $account_id, $ip, $ua_hash, time());
		if(count($devices) == 4)
		{
			$db->query("UPDATE `accounts` SET `suspended_for`='account sharing (B4)' WHERE `id`=?", "s", $account_id);
			return false;
		}
	}
	return true;
}
