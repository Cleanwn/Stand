<?php
// Sadly doesn't work, seems like it needs a ticket to create a ticket...

$ch = curl_init();
curl_setopt_array($ch, [
	CURLOPT_URL => "https://prod.ros.rockstargames.com/gta5/11/gameservices/auth.asmx/CreateTicketSc3",
	CURLOPT_RETURNTRANSFER => 1,
	CURLOPT_VERBOSE => 1,
	CURLOPT_HTTPHEADER => [
		"Content-Type" => "application/x-www-form-urlencoded; charset=utf-8"
	],
	CURLOPT_SSL_VERIFYHOST => false,
	CURLOPT_SSL_VERIFYPEER => false,
	CURLOPT_POSTFIELDS => http_build_query([
		"ticket" => "",
		"platformName" => "pcros",
		"nickname" => "",
		"email" => "YOUR EMAIL HERE",
		"password" => "YOUR PASSWORD HERE",
	])
]);
echo curl_exec($ch);
