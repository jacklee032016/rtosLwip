import json

from . import IpCmdIf

from utils import ColorMsg

class IpCommand(object):
    """demonstration class only
      - coded for clarity, not efficiency
    """

    def __init__(self, *args,  **kwargs):
        self.cmdSocket = IpCmdIf.IpCmdSocket(*args, **kwargs )

        self.sendCmds = {
            "targ": kwargs.get("target", "FF:FF:FF:FF:FF:FF"),
            "cmd": kwargs.get("command", "get_param"),
            "login-ack": kwargs.get("user", "admin"),
            "pwd-msg": kwargs.get("password", "admin"),
            "data": kwargs.get("data", None)
        }
        self.debug = kwargs.get("debug", False)


    def run(self, command="get_param", data=None, **kwargs ):
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
        
