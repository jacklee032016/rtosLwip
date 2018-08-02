import struct
import binascii
import json
import socket
import sys
import time

import logging
import abc

from utils import ColorMsg
from utils import settings


LOGGER = logging.getLogger(__name__)

class DeviceCtrl(object):
    __metaclass__ = abc.ABCMeta

    def __init__(self, *args, **kwargs):
        self.simGateway = kwargs.get("simGateway", None)
        self.debug = kwargs.get("debug", False)

        self.ip = kwargs.get("ip", None)
        if self.ip is None:
            self.ip = self.findIp()


    @abc.abstractmethod
    def _run(self):
        pass

    def run(self, *args, **kwargs):
        start_time = time.time()  # time() is float
        response = self._run(*args, **kwargs)
        self.timeused = (time.time() - start_time) * 1000  # millsecond
        ColorMsg.debug_msg("cmd \"%s\" after %s ms" % (self.__class__.__name__ ,self.timeused), self.debug)
        return response

    def  findIp(self):
        if self.simGateway is not None:
            return "192.168.166.2"
        else:
            return "192.168.168.120"


