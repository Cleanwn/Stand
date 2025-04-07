<?php
require __DIR__."/src/include.php";

header("Cache-Control: private, no-cache, max-age=0");

try
{
	checkAccess();
	$acctid = expandAccountId($_GET["acctid"]);
	$res = $db->query("SELECT `had_email` FROM `accounts` WHERE `id`=?", "s", $acctid);
	if (!$res[0]["had_email"] || $res[0]["had_email"] == "1")
	{
		http_response_code(400);
		exit;
	}

	$ch = curl_init();
	curl_setopt_array($ch, [
		CURLOPT_RETURNTRANSFER => true,
		CURLOPT_URL => "http://198.251.88.12/send-mail.php",
		CURLOPT_POST => true,
		CURLOPT_POSTFIELDS => [
			"i" => $acctid,
			"r" => $res[0]["had_email"]
		]
	]);
	$transfer = curl_exec($ch);
	$transfer = str_replace($acctid, "[redacted]", $transfer);
	$transfer = str_replace($res[0]["had_email"], "[redacted]", $transfer);
	echo "<pre>".htmlspecialchars($transfer)."</pre>";
	curl_close($ch);
}
catch (Exception $e)
{
	echo "<p>".$e->getMessage()."</p>";
}
