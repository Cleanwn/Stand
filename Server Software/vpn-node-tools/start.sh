#!/bin/bash

iptables -t nat -A POSTROUTING -o eth0 -j MASQUERADE

screen -dmS "auth-servant" sh run.sh
