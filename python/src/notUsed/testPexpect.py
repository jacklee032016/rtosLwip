
import pexpect

time = 3
conns = 5
opts = "-t %s -P %s" % (time, conns)

#   'grep -v message': ignore the message
# cmd = 'iperf -c %s %s | grep -v SUM' % ('192.168.166.2', opts)

cmd = 'iperf -c %s %s' % ('192.168.166.2', opts)

print('cmd=%s'%(cmd))
# iperf -c 192.168.166.2 -t 60 -P 5 | grep -v SUM
child = pexpect.spawn(cmd)

#child.sendline('iperf -c %s | grep -v SUM' % ('192.168.166.2'))
child.expect('Client connecting to')
#child.expect("")

rate = 0.0
for i in range(0, conns):
    m = child.expect(['Bytes([^M]*)Mbits', 'Bytes([^K]*)Kbits'], timeout=time + 30)
    if m == 0:
        rate += float(child.match.group(1))
    elif m == 1:
        rate += float(child.match.group(1)) / 1000.0
    else:
        print("Unknown units for iPerf results!\n")
        assert False

#child.expect(prompt)
print("bandwidth %f" %(rate))


