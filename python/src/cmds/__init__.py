import struct
import binascii
import json
import socket
import sys
import time

import logging
import abc

from utils import ColorMsg

import tftpy

LOGGER = logging.getLogger(__name__)

class DeviceCtrl(object):
    __metaclass__ = abc.ABCMeta

    def __init__(self, *args, **kwargs):
        self.simGateway = kwargs.get("simGateway", None)
        self.debug = kwargs.get("debug", False)

        self.ip = kwargs.get("ip", None)
        #if self.ip is None:
        #    self.ip = self.find()


    @abc.abstractmethod
    def _run(self):
        pass

    def run(self):
        start_time = time.time()  # time() is float
        response = self._run()
        self.timeused = (time.time() - start_time) * 1000  # millsecond
        ColorMsg.debug_msg("cmd \"%s\" after %s ms" % (self.__class__.__name__ ,self.timeused), self.debug)

    def  find(self):
        if self.simGateway is not None:
            return "192.168.166.2"
        else:
            return "192.168.168.120"


class TftpUpload(DeviceCtrl):

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.filename = kwargs.get("fm","../../an767/BIN/images.AN767/rtosLwipAN767.bin.bin")
        self.fmType = kwargs.get("fmType", "muxOs")

    def _run(self):
        ColorMsg.debug_msg("TFTP %s, upload %s : %s" %(self.ip, self.fmType, self.filename), self.debug )
        self.client = tftpy.TftpClient(self.ip)
        self.client.upload(self.fmType, self.filename)


class TftpFpgaUpload(TftpUpload):

    def __init__(self, *args, **kwargs):
        super(TftpFpgaUpload, self).__init__(*args, **kwargs)
        self.filename = kwargs.get("fm","../../../Binary/top_Angelica_N_767_rx1.bin")
        self.fmType = kwargs.get("fmType", "muxFpga")

