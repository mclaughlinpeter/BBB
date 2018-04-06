#!/bin/bash
echo "networkShareBB script"
iptables --table nat --append POSTROUTING --out-interface eth0 -j MASQUERADE
iptables --append FORWARD --in-interface eth1 -j ACCEPT
sh -c "echo 1 > /proc/sys/net/ipv4/ip_forward"
echo "end of networkShareBB script"

