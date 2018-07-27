#!/usr/bin/env python3

# 

import json
import time
import sys,getopt

from time import sleep

from cmds.ipCmd.IpCmd import IpCommand

def testFind(*arg, **kwargs):
    agent = IpCommand(port=kwargs.get("port", 3600), gateway=kwargs.get("gateway",None), debug=kwargs.get("debug", False) )
    result = agent.run()
    return (agent, result)

def testSysCfg(*arg, **kwargs):
    agent, result = testFind(*arg, **kwargs)

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

def testOther(*arg, **kwargs):
    agent = IpCommand(port=3600, gateway="192.168.166.1", debug=True )

    secureSetKeyCmd = [
        {
            "set_key": "1122334455"
        }
    ]

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


def usage():
    print("Usage: %s: -f --find -s --sysCfg -g gateway GATEWAY -r reboot -t reboot -d debug -h help"%(sys.argv[0]))

if __name__ == "__main__":
    print(sys.argv[0] )
    print(sys.argv[1:] )
    gateway = None
    debug = False
    cmd = "find"

    options, remainder = getopt.getopt(sys.argv[1:], 'fs:g:rdh', ['find', 'sysCfg', "gateway=", 'reboot', 'debug', "help'"])
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
        else:
            usage()


    if cmd == 'find':
        testFind(gateway=gateway, debug=debug)
    elif cmd == 'sysCfg':
        testSysCfg(gateway=gateway, debug=debug)
    else:
        testOther(gateway=gateway, debug=debug)
