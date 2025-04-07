<?php
if(empty($_GET["link_token"]))
{
	?>
	<p>If you're reading this, your browser is <span>not executing Javascript</span>. Must suck to have such a shit browser. <a href="https://stand.gg/account/">Back to account management.</a></p>
	<script>
		if(localStorage.getItem("link_token"))
		{
			document.querySelector("p").textContent="Please wait...";
			setTimeout(function()
			{
				document.querySelector("p").textContent="It's probably done already and you're just experiencing a bug with your browser.";
			},3000);
			location.href=location.href+"&link_token="+localStorage.getItem("link_token");
		}
		else
		{
			document.querySelector("span").textContent="probably not allowing cookies";
		}
	</script>
	<?php
	exit;
}
if(empty($_GET["code"]))
{
	die("Discord should've given you a code. I have no idea how you ended up in this state.");
}

header("Cache-Control: private, no-cache, max-age=0");

require __DIR__."/../common/include.php";
$res = $db->query("SELECT `id`,`privilege`,`last_known_discord_id` FROM `accounts` WHERE `link_token`=? AND `suspended_for`=''", "s", $_GET["link_token"]);
if(count($res) == 0)
{
	?>
	<p>Token has expired. <a href="https://stand.sh/account/">Back to account management.</a></p>
	<?php
	exit;
}
antisharing_tallyAccountAccess($res[0]["id"]);
$db->query("UPDATE `accounts` SET `link_token`='' WHERE `id`=?", "s", $res[0]["id"]);

// https://discord.com/oauth2/authorize?scope=bot&permissions=8&client_id=842826387501088778
//$token = "ODQyODI2Mzg3NTAxMDg4Nzc4.G16W6z.2dZbn-sxhkpdjHJm90jGPtNjfetlCbiE0CM2Ec";

// https://discord.com/oauth2/authorize?scope=bot&permissions=8&client_id=1269083011673096245
$token = "MTI2OTA4MzAxMTY3MzA5NjI0NQ.GuBmJi.RXn6kGMYQyF2JKB9OxdhXzGhVSWgJqzk1tW2Bw";

$guild_id = "956618713157763072";
$roleid_toxic = "956618713157763075";
$roleid_sane = "956618713157763074";
$roleid_basic = "956618713157763073";
$roleid_nsfwban = "956618713157763078";

/*$guild_id = "1017775626180112465";
$roleid_toxic = "1017775626180112468";
$roleid_sane = "1017775626180112467";
$roleid_basic = "1017775626180112466";
$roleid_nsfwban = "1017775626180112471";*/

function addRole($user_id, $role_id)
{
	global $guild_id, $token;

	$ch = curl_init();
	curl_setopt_array($ch, [
		CURLOPT_URL => "https://discordapp.com/api/v6/guilds/{$guild_id}/members/{$user_id}/roles/".$role_id,
		CURLOPT_RETURNTRANSFER => true,
		CURLOPT_CUSTOMREQUEST => "PUT",
		CURLOPT_POST => true,
		CURLOPT_POSTFIELDS => [],
		CURLOPT_HTTPHEADER => [
			"Authorization: Bot {$token}"
		]
	]);
	curl_exec($ch);
	curl_close($ch);
}

function removeRole($user_id, $role_id)
{
	global $guild_id, $token;

	$ch = curl_init();
	curl_setopt_array($ch, [
		CURLOPT_URL => "https://discordapp.com/api/v6/guilds/{$guild_id}/members/{$user_id}/roles/".$role_id,
		CURLOPT_RETURNTRANSFER => true,
		CURLOPT_CUSTOMREQUEST => "DELETE",
		CURLOPT_POST => true,
		CURLOPT_POSTFIELDS => [],
		CURLOPT_HTTPHEADER => [
			"Authorization: Bot {$token}"
		]
	]);
	curl_exec($ch);
	curl_close($ch);
}

$ch = curl_init();
curl_setopt_array($ch, [
	CURLOPT_URL => "https://discordapp.com/api/v6/oauth2/token",
	CURLOPT_RETURNTRANSFER => true,
	CURLOPT_POST => true,
	CURLOPT_POSTFIELDS => [
		"client_id" => "1269083011673096245",
		"client_secret" => "-7U1hdv9DuWD-dD0jkfXkNvGT3pYzQ19",
		"grant_type" => "authorization_code",
		"code" => $_GET["code"],
		"redirect_uri" => $redirect_uri
	]
]);
$json = json_decode(curl_exec($ch), true);
curl_close($ch);
if(!isset($json["access_token"]))
{
	die("Error 1");
}

$ch = curl_init();
curl_setopt_array($ch, [
	CURLOPT_URL => "https://discordapp.com/api/v6/users/@me",
	CURLOPT_RETURNTRANSFER => true,
	CURLOPT_HTTPHEADER => [
		"Authorization: Bearer ".$json["access_token"]
	]
]);
$user = json_decode(curl_exec($ch), true);
curl_close($ch);
if(!isset($user["id"]))
{
	die("Error 2");
}
