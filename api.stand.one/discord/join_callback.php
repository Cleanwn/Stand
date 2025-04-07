<?php
$redirect_uri = "https://backend-services.stand.sh/discord/join_callback.php";
require "include.php";

if($res[0]["last_known_discord_id"] != $user["id"])
{
	// Kick old account
	if($res[0]["last_known_discord_id"])
	{
		$ch = curl_init();
		curl_setopt_array($ch, [
			CURLOPT_URL => "https://discordapp.com/api/v6/guilds/".$guild_id."/members/".$res[0]["last_known_discord_id"],
			CURLOPT_CUSTOMREQUEST => "DELETE",
			CURLOPT_RETURNTRANSFER => true,
			CURLOPT_POST => true,
			CURLOPT_POSTFIELDS => [],
			CURLOPT_HTTPHEADER => [
				"Authorization: Bot {$token}"
			]
		]);
		curl_exec($ch);
		curl_close($ch);
	}

	// Update last_known_discord_id
	$db->query("UPDATE `accounts` SET `last_known_discord_id`=? WHERE `id`=?", "ss", $user["id"], $res[0]["id"]);

	// Tally discords linked to account
	$ads = $db->query("SELECT * FROM `account_discords` WHERE `account_id`=?", "s", $res[0]["id"]);
	$is_new = true;
	$num_recent_ads = 1; // 1 for the one that's joining right now
	foreach ($ads as $ad)
	{
		if ($ad["discord_id"] == $user["id"])
		{
			$is_new = false;
			//++$num_recent_ads;
			$db->query("UPDATE `account_discords` SET `last_join`=? WHERE `account_id`=? AND `discord_id`=?", "iss", time(), $res[0]["id"], $user["id"]);
			breaK;
		}
		else if ($ad["last_join"] > days_ago(45))
		{
			++$num_recent_ads;
		}
	}

	// Track this one if it's new
	if ($is_new)
	{
		$db->query("INSERT INTO `account_discords` (`account_id`, `discord_id`, `last_join`) VALUES (?, ?, ?)", "ssi", $res[0]["id"], $user["id"], time());
	}

	// Suspend if too many recent discord accounts
	if ($num_recent_ads >= 4)
	{
		$db->query("UPDATE `accounts` SET `suspended_for`='account sharing or ban evasion' WHERE `id`=?", "s", $res[0]["id"]);
	}
}

// Add to guild with role
$roles = [];
if($res[0]["privilege"] == 1)
{
	$roles = [$roleid_basic];
}
else if($res[0]["privilege"] == 2)
{
	$roles = [$roleid_sane];
}
else if($res[0]["privilege"] == 3)
{
	$roles = [$roleid_toxic];
}
$ch = curl_init();
curl_setopt_array($ch, [
	CURLOPT_URL => "https://discordapp.com/api/v6/guilds/".$guild_id."/members/".$user["id"],
	CURLOPT_CUSTOMREQUEST => "PUT",
	CURLOPT_RETURNTRANSFER => true,
	CURLOPT_POST => true,
	CURLOPT_POSTFIELDS => json_encode([
		"access_token" => $json["access_token"],
		"roles" => $roles
	]),
	CURLOPT_HTTPHEADER => [
		"Authorization: Bot {$token}",
		"Content-Type: application/json"
	]
]);
echo "There you go. ".curl_exec($ch);
curl_close($ch);
?>
<a href="https://stand.sh/account/">Back to account management.</a>
