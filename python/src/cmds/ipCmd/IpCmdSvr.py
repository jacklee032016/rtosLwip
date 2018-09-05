import json
import time
import sys

from cmds import DeviceCtrl

from . import *
from . import IpCmdIf
from . import CommandCodec

from utils import ColorMsg
from utils import settings

class IpCommandSvr(DeviceCtrl):
    """demonstration class only
      - coded for clarity, not efficiency
    """

    def __init__(self, *args,  **kwargs):
        super().__init__(self, *args, **kwargs)
        self.cmdSocket = IpCmdIf.IpCmdSocket(self, *args, **kwargs )

        self.replyCmds = {
            "targ": kwargs.get("target", "FF:FF:FF:FF:FF:FF"),
            "cmd": kwargs.get("command", "get_param"),
            "login-ack": kwargs.get("user", "admin"),
            "pwd-msg": kwargs.get("password", "admin"),
            "data": kwargs.get("data", None)
        }

        ColorMsg.debug_msg("set sendCmds", debug=True)
        self.server = kwargs.get("ip", "<broadcast>")
        self.port = kwargs.get("port", 3840)
        self.debug = kwargs.get("debug", False)
        #self.simGateway = kwargs.get("simGateway", None)

        self.peer = (self.server, self.port)

        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        # Set a timeout so the socket does not block indefinitely when trying to receive data.
        #self.sock.settimeout(3.0)

        ColorMsg.debug_msg( '%s, %s:%s'%(self.__class__.__name__, self.server, self.port ), self.debug)
        try:
            self.sock.bind((self.server, self.port))
            #self.sock.listen(10)
        except socket.error as msg:
            ColorMsg.error_msg("BIND failed: ".format(msg))
            # sys.exit()



    def _run(self, command="get_param", data=None, **kwargs ):
        self.replyCmds['cmd'] = command
        self.count = 0

        if data is not None:
            self.replyCmds['data'] = data
        else:
            self.replyCmds['data'] = []

        #self.replyCmds['targ'] = kwargs.get("target", "FF:FF:FF:FF:FF:FF")
        #self.replyCmds['811Ip'] = kwargs.get("811Ip", "192.168.168.102")
        #self.replyCmds['811Port'] = kwargs.get("811Port", 3840 )
        #json_string = json.dumps(self.sendCmds)

        start_time = time.time()  # time() is float
        while True:
            data, addr = self.sock.recvfrom(2018)
            self.count = self.count+1
            cmdJson = CommandCodec.decode(data, self.debug)

            #self.timeused = (time.time() - start_time) * 1000  # millsecond
            ColorMsg.debug_msg('recv #.%d: %s from %s\n' %(self.count, cmdJson, addr), self.debug )
            self.replyCmds['targ'] = cmdJson['targ']
            self.replyCmds['cmd'] = cmdJson['cmd']
            self.replyCmds['login-ack'] = "OK"
            self.replyCmds['pwd-msg'] = "OK"

            reply = CommandCodec.encode( self.replyCmds, debug=self.debug)
            ColorMsg.debug_msg('reply #.%d: %s to %s\n' %(self.count, self.replyCmds, addr), self.debug )
            self.sock.sendto(reply, addr )

        return None
