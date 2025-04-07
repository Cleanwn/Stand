<?php
$myhostname = gethostbyaddr(file_get_contents("https://ipv4.apimon.de/"));
echo "Detected hostname: $myhostname\n";

passthru("apt-get install -y php-cli php-curl screen certbot strongswan strongswan-pki libcharon-extra-plugins libcharon-extauth-plugins libstrongswan-extra-plugins");

passthru("certbot certonly -m letsencrypt@sai.navy --agree-tos -d $myhostname --manual --preferred-challenges dns");
copy("/etc/letsencrypt/live/$myhostname/chain.pem", "/etc/ipsec.d/cacerts/chain.pem");

file_put_contents("/etc/ipsec.conf", str_replace("\r", "", <<<EOC
# ipsec.conf - strongSwan IPsec configuration file

config setup
	charondebug="ike 0, enc 0, net 0, cfg 0"
	uniqueids=no

conn ikev2-vpn
	auto=add
	compress=no
	type=tunnel
	keyexchange=ikev2
	fragmentation=yes
	forceencaps=yes
	dpdaction=clear
	dpddelay=300s
	ike=chacha20poly1305-sha512-curve25519-prfsha512,aes256gcm16-sha384-prfsha384-ecp384,aes256-sha1-modp1024,aes128-sha1-modp1024,3des-sha1-modp1024
	esp=chacha20poly1305-sha512,aes256gcm16-ecp384,aes256-sha256,aes256-sha1,3des-sha1
	rekey=no
	left=%any
	leftid=@$myhostname
	leftauth=pubkey
	leftcert=/etc/letsencrypt/live/$myhostname/cert.pem
	leftsendcert=always
	leftsubnet=0.0.0.0/0
	right=%any
	rightid=%any
	rightauth=eap-mschapv2
	rightsendcert=never
	rightsourceip=13.37.0.0/16
	eap_identity=%identity

EOC));

file_put_contents("/etc/strongswan.conf", str_replace("\r", "", <<<EOC
# strongswan.conf - strongSwan configuration file
#
# Refer to the strongswan.conf(5) manpage for details
#
# Configuration changes should be made in the included files

charon {
	load_modular = yes
	plugins {
		include strongswan.d/charon/*.conf
	}
	dns1 = 8.8.8.8
	dns2 = 1.1.1.1
	nbns1 = 8.8.8.8
	nbns2 = 1.1.1.1
}

include strongswan.d/*.conf

EOC));

file_put_contents("/etc/sysctl.conf", file_get_contents("/etc/sysctl.conf")."\nnet.ipv4.ip_forward=1\nnet.ipv4.conf.all.accept_redirects = 0\nnet.ipv4.conf.all.send_redirects = 0\nnet.ipv4.ip_no_pmtu_disc = 1\n");
passthru("sysctl -w net.ipv4.ip_forward=1");
passthru("sysctl -w net.ipv4.conf.all.accept_redirects=0");
passthru("sysctl -w net.ipv4.conf.all.send_redirects=0");
passthru("sysctl -w net.ipv4.ip_no_pmtu_disc=1");

passthru("ipsec restart");

passthru("sh start.sh");
