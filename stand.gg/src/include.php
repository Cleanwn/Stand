<?php
require __DIR__."/dbinclude.php";
require __DIR__."/geninclude.php";

function parseKey($key)
{
	if(substr($key, 0, 6) == "Stand-")
	{
		$key = substr($key, 6);
		if(substr($key, 0, 8) == "Upgrade-")
		{
			$key = substr($key, 8);
			if(strlen($key) == 31)
			{
				return ["upgrade", $key];
			}
		}
		else if(substr($key, 0, 6) == "Topup-")
		{
			$key = substr($key, 6);
			if(strlen($key) == 31)
			{
				return ["topup", $key];
			}
		}
		else
		{
			if(strlen($key) == 30)
			{
				return ["license", $key];
			}
		}
	}
	return ["", ""];
}

function isKeyTypeViableForTopup($type)
{
	return $type == "topup";
}

function getKeyCoinsValue($parsed_key)
{
	global $db;

	if($parsed_key[0] == "topup")
	{
		$res = $db->query("SELECT `coins` FROM `topup_keys` WHERE `key`=? AND `used_by`=''", "s", $parsed_key[1]);
		if(count($res) == 1)
		{
			return intval($res[0]["coins"]);
		}
	}
	return 0;
}

function setKeyUsedForTopup($parsed_key, $account_id)
{
	global $db;

	if($parsed_key[0] == "topup")
	{
		$db->query("UPDATE `topup_keys` SET `used_by`=? WHERE `key`=?", "ss", $account_id, $parsed_key[1]);
	}
}

function days_ago($d)
{
	return (time() - (60 * 60 * 24 * $d));
}

function pubSubSend($topic, $msg)
{
	$ch = curl_init();
	curl_setopt_array($ch, [
		CURLOPT_URL => "http://198.251.89.45/pub?topic=".$topic,
		CURLOPT_POST => true,
		CURLOPT_POSTFIELDS => $msg,
		CURLOPT_RETURNTRANSFER => true,
	]);
	curl_exec($ch);
	curl_close($ch);
}
