import json

from cmds import DeviceCtrl

from . import *
from . import IpCmdIf


from utils import ColorMsg

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

        res = result
        if result['login-ack'] != "OK":
            ColorMsg.error_msg("\tIP Command '%s' failed %s\n" %(self.sendCmds["cmd"], res['pwd-msg']))
        else:
            ColorMsg.success_msg("\tIP Command '%s' success\n" %(self.sendCmds["cmd"]))
        return result
        

class DeviceIpCmd(IpCommand):

    def __init__(self, *args, **kwargs):
        super(DeviceIpCmd, self).__init__(*args, **kwargs)

    def find(self):
        self.nodes = self.run()

    def runOneNode(self, command, data, targId):
        ColorMsg.debug_msg('IP Command "%s" on target "%s"'%(command, targId), self.debug)
        return self.run(command=command, data=data, target=targId)

    def secureSetKey(self, *args, **kwargs):
        data = kwargs.get('data', "1122334455")
        secureSetKeyCmd = [
            {
                "set_key": data
            }
        ]

        node = kwargs.get('targ', None)
        if node is not None:
            return self.runOneNode(command=IP_CMD_SECURE, data=secureSetKeyCmd, target=node)

        self.find()
        for node in self.nodes:
            self.runOneNode(command=IP_CMD_SECURE, data=secureSetKeyCmd, target=node["targ"])


    def secureGetId(self, *args, **kwrgs):
        pass

    def secureGetStatus(self):
        self.find()
        secureGetIdCmd = [
            {
                "get_id": ""
            }
        ]
        pass

    def rs232Data(self, *args, **kwargs):
        data = kwargs.get('data', "1122334455")
        rs232Cmd = [
            {
                "hexdata": data,
                "isFeedback": 1,
                "waitTime": 2
            }
        ]

        node = kwargs.get('targ', None)
        if node is not None:
            return self.runOneNode(command=IP_CMD_RS232_DATA, data=rs232Cmd, target=node)

        self.find()
        for node in self.nodes:
            self.runOneNode(command=IP_CMD_RS232_DATA, data=rs232Cmd, target=node["targ"])
