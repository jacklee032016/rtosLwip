import pexpect

time = 3
conns = 5
opts = "-t %s -P %s" % (time, conns)

logfile = 'result.txt'

#cmd = 'scp %s %s@%s:%s' % (dest, remote_uname, remote_ip, src)
cmd = 'iperf -c %s %s | grep  SUM' % ('192.168.166.2', opts)

print("Running cmd: %s" % (cmd))
child = pexpect.spawn(cmd)
try:
    child.logfile = open(logfile, "a")
except Exception as e:
    child.logfile = None

child.logfile = None
#child.expect('Client connecting to')
#child.sendline(remote_passwd)
try:
    child.expect(pexpect.EOF)
    out = child.before
    print("\"%s\""%(str(out)) )
    if 'SUM' in str(out):
        print('True')
    else:
        print('False')
except Exception as e:
     print("Import error, error : '%s'" % str(e))
     #return False
