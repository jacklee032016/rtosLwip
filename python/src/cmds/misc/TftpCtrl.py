
from utils import tftp

from cmds import DeviceCtrl

from utils import ColorMsg
from utils import settings


class TftpUpload(DeviceCtrl):

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

    def _run(self, *argd, **kwargs):
        self.fmType = kwargs.get("fmType", None)
        if self.fmType is None or self.fmType == settings.FIRMWARE_TYPE_TFTP_OS:
            self.fmType = settings.FIRMWARE_TYPE_TFTP_OS
            self.filename = kwargs.get("file", settings.FIRMWARE_BIN_OS)
        elif self.fmType  == settings.FIRMWARE_TYPE_TFTP_FPGA:
            self.fmType = settings.FIRMWARE_TYPE_TFTP_FPGA
            self.filename = kwargs.get("file", settings.FIRMWARE_BIN_FPGA_RX)
        else:
            ColorMsg.error_msg("FirmType is error %s"%self.fmType)
            return

        ColorMsg.debug_msg("TFTP %s, upload %s : %s" %(self.ip, self.fmType, self.filename), self.debug )
        self.client = tftp.TftpClient(self.ip)
        self.client.upload(self.fmType, self.filename, timeout=5)

    def uploadOs(self, *args, **kwargs):
        self.run(*args, fmType=settings.FIRMWARE_TYPE_TFTP_OS, **kwargs)

    def uploadFpga(self, *args, **kwargs):
        self.run(*args, fmType=settings.FIRMWARE_TYPE_TFTP_FPGA, **kwargs)