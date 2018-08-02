#!/usr/bin/env python

"""Helper script to run all IP commands.

usage: ipCmds.py [options]

options:
    -f, --find     Find all nodes in LAN
    -t, --tftp firmOs|firmFpga :    tftp firmware update
    -r, --reboot   reboot all nodes
    -s, --syscfg   system configuration, DHCP, IP, pName, etc.
    -d, --debug    enable debug
    -g, --gateway   gateway address, used only simulation environment
    -h, --help      Show help
"""

# import getopt
import sys
import argparse

from cmds.http.web import WebClient
from cmds.ipCmd.IpCmdCtrl import DeviceIpCmd
from cmds.misc.TftpCtrl import TftpUpload
from cmds.misc.iperf import Iperf
from utils import ColorMsg


def bandwidth(*args, **kwargs):
    client = Iperf(connections=5, timeout=3, **kwargs)
    ip, rate = client.getRate()
    print("Bandwidth of %s is %d Mbps" % (ip, rate))

def findAll(*args, **kwargs):
    agent = DeviceIpCmd(*args, **kwargs )
    nodes = agent.find()
    for node in nodes:
        data = node["data"][0]
        ColorMsg.test_msg("%s: %s %s, %s"%(data["pName"], data["ip"], data["mac"], data["model"]))

def testSysCfg(*arg, **kwargs):
    agent = DeviceIpCmd(*arg, **kwargs)
    nodes = agent.find()

    if len(nodes) == 0:
        ColorMsg.error_msg("No node is found in LAN")
        return

    setupSystemCmd = [
        {
            "cName": "SimTx",
            "mac": "00:04:25:1c:10:03",
            "ip": "192.168.168.120",
            "isDhcp": 0,
            "isDipOn": 0
        }
    ]

    for node in nodes:
        data = node["data"][0]
        agent.sysConfig(data=setupSystemCmd, ip=data["ip"], target=node["targ"])


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


def tftpUpdate(fmType, debug):
    #tftpmcu = TftpUpload(simGateway="192.168.166.1", debug=True)
    tftpmcu = TftpUpload(debug=True)
    tftpmcu.run(fmType)


def httpUpdate(*args, **kwargs):
    webClient = WebClient(*args, **kwargs)
    webClient.uploadFile(*args, **kwargs)

    reboot()

def reboot(*args, **kwargs):
    webClient = WebClient(*args, **kwargs)
    webClient.reboot()

def accessPage(*args, **kwargs):
    uri = kwargs.get("uri", None)
    if uri is None:
        ColorMsg.error_msg("No URI is defined")
        return
    webClient = WebClient(*args, **kwargs)
    webClient.accessUri(*args, **kwargs)

def parse_args(argv):
    try:
        options, remainder = getopt.getopt(sys.argv[1:], 'fct:r:g:bu:p:w:dh', ['find', 'cfg', 'tftp', 'rs232', 'gateway','update', 'boot', 'debug', 'help'])
        #if remainder:
        #    raise getopt.error('no argument accepted, got %s'%list(remainder) )

        #print('OPTIONS   :',options )
        #print('REMAINDER   :',remainder  )

    except getopt.error as err:
        usage_exit(err)

    if len(options) == 0 or options is None:
        usage_exit("no argument")
        sys.exit(1)

    return options


def usage_exit(msg=None):
    print(__doc__)
    if msg is None:
        rc = 251
    else:
        print('\nError:', msg)
        rc = 252
    sys.exit(rc)

class Run(object):

    def __init__(self):
        parser = argparse.ArgumentParser(
            description='Pretends to be git',
            usage='''git <command> [<args>]

The most commonly used git commands are:
   commit     Record changes to the repository
   fetch      Download objects and refs from another repository
''')
        parser.add_argument('command', help='Subcommand to run')
        # parse_args defaults to [1:] for args, but you need to
        # exclude the rest of the args too, or validation will fail
        args = parser.parse_args(sys.argv[1:2])
        if not hasattr(self, args.command):
            print('Unrecognized command')
            parser.print_help()
            exit(1)
        # use dispatch pattern to invoke method with same name
        getattr(self, args.command)()

    def commit(self):
        parser = argparse.ArgumentParser(
            description='Record changes to the repository')
        # prefixing the argument with -- means it's optional
        parser.add_argument('--amend', action='store_true')
        # now that we're inside a subcommand, ignore the first
        # TWO argvs, ie the command (git) and the subcommand (commit)
        args = parser.parse_args(sys.argv[2:])
        print('Running git commit, amend=%s' % args.amend)

    def fetch(self):
        parser = argparse.ArgumentParser(
            description='Download objects and refs from another repository')
        # NOT prefixing the argument with -- means it's not optional
        parser.add_argument('repository')
        args = parser.parse_args(sys.argv[2:])
        print('Running git fetch, repository=%s' % args.repository)


if __name__ == "__main__":
    print(sys.argv[0] )
    print(sys.argv[1:] )
    gateway = None
    debug = False
    cmd = "find"

    Run()

#    parser = argparse.ArgumentParser(description='Helper script to run all device control commands.')

    sys.exit(1)

    # options = parse_args(sys.argv[1:])

    if len(options) == 0 or options is None:
        usage_exit()
        sys.exit(1)

    for opt, arg in options:
        print(opt, arg)
        if opt in ('-f', '--find'):
            cmd = "find"
        elif opt in ('-c', '--cfg'):
            cmd = "cfg"
        elif opt in ('-t', '--tftp'):
            cmd = "tftp"
            data = arg
        elif opt in ('-r', '--rs232'):
            cmd = "rs232"
            data = arg
        ## command from http service
        elif opt in ('-b', '--boot'):
            cmd = "reboot"
        elif opt in ('-u', '--update'):
            cmd = "update"
            data = arg
        elif opt in ('-p', '--page'):
            cmd = "page"
            data = arg
        elif opt in ('-w', '--width'):
            cmd = "bandwidth"
            data = arg
        elif opt in ('-g', '--gateway'):
            gateway = arg
        elif opt in ('-o', '--others'):
            testOpt = arg
        elif opt in ('-d', '--debug'):
            debug = True
        else:
            usage_exit()


    if cmd == 'find':
        findAll(simGateway=gateway, debug=debug)
    elif cmd == 'cfg':
        testSysCfg(gateway=gateway, debug=debug)
    elif cmd == 'tftp':
        tftpUpdate(data, debug=debug)
    elif cmd == 'reboot':
        reboot(debug=debug)
    elif cmd == 'update':
        httpUpdate(data, debug=debug)
    elif cmd == 'bandwidth':
        bandwidth(ip=data, debug=debug)
    elif cmd == 'page':
        accessPage(uri=data, debug=debug)
    else:
        testOther(gateway=gateway, debug=debug)
