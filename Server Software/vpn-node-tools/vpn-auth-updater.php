<?php
$myhostname = gethostbyaddr(file_get_contents("https://ipv4.apimon.de/"));
$last_users = "";

echo "$myhostname is up and running!\n";

function getNumConnections()
{
	foreach(explode("\n", shell_exec("ipsec statusall")) as $line)
	{
		if(substr($line, 0, 23) == "Security Associations (")
		{
			return intval(substr($line, 23));
		}
	}
	return 0;
}

while(true)
{
	$clients = getNumConnections();
	echo "Syncing at $clients clients...";
	$ch = curl_init();
	curl_setopt_array($ch, [
		CURLOPT_URL => "https://stand.gg/vpn/authsync",
		CURLOPT_RETURNTRANSFER => true,
		CURLOPT_POSTFIELDS => http_build_query([
			"key" => '#8%o6!nLGFZE8fNBS89$SjHrr59bL!Ra$vu4JD7n44NrKkt8Tx3^fohfUVh8pwD9FkiX&%@E',
			"hostname" => $myhostname,
			"clients" => $clients,
		])
	]);
	$users = curl_exec($ch);
	curl_close($ch);
	if($users == $last_users)
	{
		echo " Still up-to-date.\n";
	}
	else
	{
		$last_users = $users;
		$users = explode(",", $users);
		$c = count($users);
		if($c == 0)
		{
			break;
		}
		echo " Need update, remote reports ".$c." users...";
		$cont = ": RSA /etc/letsencrypt/live/$myhostname/privkey.pem\n\n";
		foreach($users as $user)
		{
			$cont .= "Stand-Activate-{$user} : EAP \"a\"\n";
		}
		file_put_contents("/etc/ipsec.secrets", $cont);
		shell_exec("ipsec rereadsecrets");
		echo " Done.\n";
	}
	sleep(5 * 60);
}
