<?php
$myhostname = gethostbyaddr(file_get_contents("https://ipv4.apimon.de/"));
echo "Detected hostname: $myhostname\n";

passthru("certbot certonly -m letsencrypt@sai.navy --agree-tos -d $myhostname --manual --preferred-challenges dns");
copy("/etc/letsencrypt/live/$myhostname/chain.pem", "/etc/ipsec.d/cacerts/chain.pem");

passthru("ipsec reload");
