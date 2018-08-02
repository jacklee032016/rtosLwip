#!/usr/bin/env python
# 

import json
import time
import sys,getopt

from time import sleep

from cmds import TftpUpload, TftpFpgaUpload

from cmds.ipCmd.IpCmd import DeviceIpCmd

def testFind(*args, **kwargs):
    agent = DeviceIpCmd(*args, **kwargs )
    result = agent.find()

def testSysCfg(*arg, **kwargs):
    agent = DeviceIpCmd(*arg, **kwargs)

    setupSystemCmd = [
        {
            "cName": "SimTx",
            "mac": "00:04:25:1c:10:02",
            "ip": "192.168.168.120",
            "isDhcp": 1,
            "isDipOn": 0
        }
    ]

    agent.run(command="set_param", data=setupSystemCmd, target=result["targ"])

def testOther(*args, **kwargs):
    agent = DeviceIpCmd(*args, **kwargs )
    agent.rs232Data(data="1122334455")

    secureGetIdCmd = [
        {
            "get_id": ""
        }
    ]

    secureGetStatusCmd = [
        {
            "get_status": ""
        }
    ]


    setupSystemCmd = [
        {
            "cName": "SimTx",
            "mac": "00:04:25:1c:10:02",
            "ip": "192.168.166.3",
            "isDhcp": 1,
            "isDipOn": 0
        }
    ]

    result = agent.run()

    agent.run(command="set_param", data=setupSystemCmd, target=result["targ"])


def testTftp():
    tftpmcu = TftpUpload(simGateway="192.168.166.1", debug=True)
    tftpmcu.run()

    tftpFpga = TftpFpgaUpload(simGateway="192.168.166.1", debug=True)
    tftpFpga.run()

def usage():
    print("Usage: %s: -f --find -s --sysCfg -g gateway GATEWAY -r reboot -t reboot -d debug -h help"%(sys.argv[0]))

if __name__ == "__main__":
    print(sys.argv[0] )
    print(sys.argv[1:] )
    gateway = None
    debug = False
    cmd = "find"

    options, remainder = getopt.getopt(sys.argv[1:], 'fs:g:rdht', ['find', 'sysCfg', 'gateway=', 'reboot', 'debug', 'help', 'tftp'])
    if len(options) == 0 or options is None:
        usage()
        sys.exit(1)

    print('OPTIONS   :',options )
    print('REMAINDER   :',remainder  )

    for opt, arg in options:
        print(opt, arg)
        if opt in ('-f', '--find'):
            cmd = "find"
        elif opt in ('-s', '--sysCfg'):
            cmd = "sysCfg"
        elif opt in ('-g', '--gateway'):
            gateway = arg
        elif opt in ('-o', '--others'):
            testOpt = arg
            testOther()
        elif opt in ('-d', '--debug'):
            debug = True
        elif opt in ('-t', '--debug'):
            cmd = "tftp"
        else:
            usage()


    if cmd == 'find':
        testFind(simGateway=gateway, debug=debug)
    elif cmd == 'tftp':
        testTftp(debug=debug)
    elif cmd == 'sysCfg':
        testSysCfg(gateway=gateway, debug=debug)
    else:
        testOther(gateway=gateway, debug=debug)
