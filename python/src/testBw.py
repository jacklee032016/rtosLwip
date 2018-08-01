#!/usr/bin/env python
#  test bandwitdh

from cmds.misc.iperf import Iperf

ip = '192.168.168.120'
client = Iperf(ip=ip)
rate = client.getRate()

print("Bandwidth of %s is %d Mbps"%(ip,rate))

