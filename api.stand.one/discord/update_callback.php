<?php
$redirect_uri = "https://backend-services.stand.sh/discord/update_callback.php";
require "include.php";

if($res[0]["last_known_discord_id"] != $user["id"])
{
	die("You need to join our Discord before you can use this.");
}

// Update role
if($res[0]["privilege"] == 3)
{
	addRole($user["id"], $roleid_toxic);
	removeRole($user["id"], $roleid_sane);
	removeRole($user["id"], $roleid_basic);
}
else if($res[0]["privilege"] == 2)
{
	addRole($user["id"], $roleid_sane);
	removeRole($user["id"], $roleid_basic);
}
else if($res[0]["privilege"] == 1)
{
	addRole($user["id"], $roleid_basic);
}
?>
<p>There you go. <a href="https://stand.sh/account/">Back to account management.</a></p>
