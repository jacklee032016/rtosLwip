# only iperf2 is support on AN767, so use pexpect replacing iperf python library
#

import pexpect

from utils import ColorMsg

time = 3
conns = 5
cmdOpts = "-t %s -P %s" % (time, conns)

class Iperf(pexpect.spawn):

    def get_log(self):
        return self._logfile_read.log

    def get_logfile_read(self):
        if hasattr(self, "_logfile_read"):
            return self._logfile_read
        else:
            return None

    def __init__(self, connections=conns, timeout=time, **kwargs):
        #   'grep -v message': ignore the message
        # cmd = 'iperf -c %s %s | grep -v SUM' % ('192.168.166.2', opts)
        self.ip = kwargs.get("ip", "192.168.168.120")
        cmd = 'iperf -c %s %s' % (self.ip, cmdOpts)
        ColorMsg.debug_msg('%s connection:%d time:%d' % (self.ip, connections, timeout), True)
        super().__init__(cmd)

    def getRate(self):
        self.expect('Client connecting to')
        # child.expect("")

        rate = 0.0
        for i in range(0, conns):
            m = self.expect(['Bytes([^M]*)Mbits', 'Bytes([^K]*)Kbits'], timeout=time + 30)
            if m == 0:
                rate += float(self.match.group(1))
            elif m == 1:
                rate += float(self.match.group(1)) / 1000.0
            else:
                ColorMsg.error_msg("Unknown units for iPerf results!\n")
                assert False

        # child.expect(prompt)
        print("bandwidth %d Mbps" % (rate))
        return self.ip, rate

# iperf -c 192.168.166.2 -t 60 -P 5 | grep -v SUM
# child = pexpect.spawn(cmd)

#child.sendline('iperf -c %s | grep -v SUM' % ('192.168.166.2'))

