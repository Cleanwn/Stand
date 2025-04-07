<?php
$payload = json_decode(file_get_contents("php://input"), true);

if(substr($payload["event"], 0, 6) != "order:")
{
	exit;
}

$message = $payload["data"]["customer_email"]." **".substr($payload["event"], 6)."** order for ".$payload["data"]["quantity"]."x ".$payload["data"]["product_title"]." totaling **".number_format($payload["data"]["total_display"], 2, ",", ".")."€.**";

$ch = curl_init();
curl_setopt_array($ch, [
	CURLOPT_URL => "https://media.guilded.gg/webhooks/5536cd64-6fcf-4fe2-ad82-3c214a67b03a/Fkb1JiBhss4CQCykQokSWG22Y2CAwGse6WOIg6Oe2e20scmk4CWiqEkYkmC8UaGGoU0KeMususqyyo0Yi6o6ss",
	CURLOPT_POST => true,
	CURLOPT_POSTFIELDS => "content=".rawurlencode($message),
	CURLOPT_RETURNTRANSFER => true
]);
curl_exec($ch);
curl_close($ch);
