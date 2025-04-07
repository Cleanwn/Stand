#!/bin/bash

apt-get update
apt-get remove -y man-db apparmor
apt-get upgrade -y
apt-get install -y php-cli php-curl screen certbot strongswan strongswan-pki libcharon-extra-plugins libcharon-extauth-plugins libstrongswan-extra-plugins iptables

php setup.php
