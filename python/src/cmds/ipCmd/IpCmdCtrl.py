import json

from cmds import DeviceCtrl

from . import *
from . import IpCmdIf


from utils import ColorMsg
from utils import settings

class IpCommand(DeviceCtrl):
    """demonstration class only
      - coded for clarity, not efficiency
    """

    def __init__(self, *args,  **kwargs):
        super().__init__(self, *args, **kwargs)
        self.cmdSocket = IpCmdIf.IpCmdSocket(self, *args, **kwargs )

        self.sendCmds = {
            "targ": kwargs.get("target", "FF:FF:FF:FF:FF:FF"),
            "cmd": kwargs.get("command", "get_param"),
            "login-ack": kwargs.get("user", "admin"),
            "pwd-msg": kwargs.get("password", "admin"),
            "data": kwargs.get("data", None)
        }
        ColorMsg.debug_msg("set sendCmds", debug=True)


    def _run(self, command="get_param", data=None, **kwargs ):
        self.sendCmds['cmd'] = command

        if data is not None:
            self.sendCmds['data'] = data
        else:
            self.sendCmds['data'] = []

        self.sendCmds['targ'] = kwargs.get("target", "FF:FF:FF:FF:FF:FF")
        #json_string = json.dumps(self.sendCmds)

        msg = self.cmdSocket.send(cmd=self.sendCmds)
        result = self.cmdSocket.receive()
        ColorMsg.debug_msg('JSON result type %s: "%s"\n' %(type(result), result), self.debug )

        for res in result:
            if res['login-ack'] != "OK":
                ColorMsg.error_msg("\tIP Command '%s' failed %s\n" %(self.sendCmds["cmd"], res['pwd-msg']))
            else:
                ColorMsg.success_msg("\tIP Command '%s' success\n" %(self.sendCmds["cmd"]))

        return result
        

class DeviceIpCmd(IpCommand):

    def __init__(self, *args, **kwargs):
        super(DeviceIpCmd, self).__init__(*args, **kwargs)
        self.nodes = None

    def find(self):
        self.nodes = self.run()
        return self.nodes

    def runOneNode(self, command, data, target):
        ColorMsg.debug_msg('IP Command "%s" on target "%s"'%(command, target), True)
        return self.run(command=command, data=data, target=target)

    def findOneNode(self, *args, **kwargs):
        self.ip = kwargs.get("ip", None)
        if self.ip is None:
            ColorMsg.error_msg("Node IP address is not defined")
            return None
        self.find()
        for node in self.nodes:
            if self.ip == node["data"][0]["ip"]:
                return node
        return None

    def secureSetKey(self, *args, **kwargs):
        data = kwargs.get('data', "1122334455")
        secureSetKeyCmd = [
            {
                "set_key": data
            }
        ]

        node = self.findOneNode(*args, **kwargs)
        if node is None:
            ColorMsg.error_msg("Node with IP %s is not found"%self.ip)

        for node in self.nodes:
            return self.runOneNode(command=settings.IP_CMD_SECURE, data=secureSetKeyCmd, target=node["targ"])


    def secureGetId(self, *args, **kwargs):
        secureGetIdCmd = [
            {
                "get_id": ""
            }
        ]
        node = self.findOneNode(*args, **kwargs)
        if node is None:
            ColorMsg.error_msg("Node with IP %s is not found"%self.ip)

        for node in self.nodes:
            return self.runOneNode(command=settings.IP_CMD_SECURE, data=secureGetIdCmd, target=node["targ"])


    def secureGetStatus(self, *args, **kwargs):
        secureGetStatusCmd = [
            {
                "get_status": ""
            }
        ]
        node = self.findOneNode(*args, **kwargs)
        if node is None:
            ColorMsg.error_msg("Node with IP %s is not found"%self.ip)

        for node in self.nodes:
            return self.runOneNode(command=settings.IP_CMD_SECURE, data=secureGetStatusCmd, target=node["targ"])

    def rs232Data(self, *args, **kwargs):
        data = kwargs.get('data', "1122334455")
        rs232Cmd = [
            {
                "hexdata": data,
                "isFeedback": 1,
                "waitTime": 2
            }
        ]

        node = self.findOneNode(*args, **kwargs)
        if node is None:
            ColorMsg.error_msg("Node with IP %s is not found"%self.ip)

        ColorMsg.debug_msg("Rs232 on:"+ node["targ"], True)
        self.runOneNode(command=settings.IP_CMD_RS232_DATA, data=rs232Cmd, target=node["targ"])

    def sysConfig(self, *args, **kwargs):
        data = kwargs.get('data', None)
        if data is None:
            ColorMsg.error_msg("No data is defined for \"%s\""%(settings.IP_CMD_SET_PARAM) )
            return

        node = self.findOneNode(*args, **kwargs)
        if node is None:
            ColorMsg.error_msg("Node with IP %s is not found"%self.ip)

        self.runOneNode(command=settings.IP_CMD_SET_PARAM, data=data, target=node["targ"])

    def setParams(self, *args, **kwargs):
        data = kwargs.get('data', None)
        if data is None:
            ColorMsg.error_msg("No data is defined for \"%s\""%(settings.IP_CMD_SET_PARAM) )
            return

        node = self.findOneNode(*args, **kwargs)
        if node is None:
            ColorMsg.error_msg("Node with IP %s is not found"%self.ip)

        self.runOneNode(command=settings.IP_CMD_SET_PARAM, data=data, target=node["targ"])
