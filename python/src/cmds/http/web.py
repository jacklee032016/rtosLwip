
import re

from .client import HttpClient

from utils import ColorMsg
from utils import settings

class WebClient(HttpClient):

    def __init__(self, *args, **kwargs):
        super().__init__( *args, **kwargs)

    def createUrl(self, *args, **kwargs):
        self.url = "http://"+ kwargs.get("ip", "192.168.168.120")+":"+str(kwargs.get("port", 80))+kwargs.get("uri", "/")
        # print("%s is called"%(self.__class__.__name__) )

    def parseResponse(self, resp, *args, **kwargs):
        self.number = "0"
        #if self.assertContentType(resp, "text/html"):
        if "text/" in resp.headers['Content-Type']:
            ColorMsg.debug_msg(resp.content.decode("utf-8"), self.debug)
            ns = re.findall("Result:(\w+)", resp.content.decode("utf-8"))
            if len(ns) > 0:  # list
                self.number = ns[0]

        #elif self.assertContentType(resp, "image/jpeg"):
        elif 'image/' in resp.headers['Content-Type']:
            ColorMsg.debug_msg(resp.content, self.debug)
        elif self.assertContentType(resp, "application/javascript"):
            ColorMsg.debug_msg(resp.content, self.debug )
        #elif self.assertContentType(resp, "text/css"):
        #    ColorMsg.debug_msg(resp.content, self.debug )
        #    return resp
        else:
            print("ELSE ")
            ColorMsg.error_msg("Unknown content type \"%s\""%(resp.headers['Content-Type']))

        return super().parseResponse( resp, *args, **kwargs)


    def uploadFile(self, *args, **kwargs):
        uri = kwargs.get("type", None)
        if uri is None or uri == settings.FIRMWARE_URL_OS:
            uri = settings.FIRMWARE_URL_OS
            fmfile = settings.FIRMWARE_BIN_OS
        elif uri == settings.FIRMWARE_URL_FPGA:
            fmfile = settings.FIRMWARE_BIN_FPGA_RX
        else:
            ColorMsg.error_msg("\"%\" is not validate URI"%(uri))
            return

        try:
            _filename = kwargs.get("file", None) # client assign it to None
            if _filename is None:
                _filename = fmfile

            fileObj = open(_filename, 'rb')
        except Exception as e:
            ColorMsg.error_msg("Open firmware file \"%s\" failed: %s:%s"%(_filename, str(e), repr(e)) )
            return

        ColorMsg.debug_msg('"%s" is uploading to %s.....'%(_filename, uri), self.debug)
        files = {'file': fileObj}
        res = self.post(*args, uri=uri, files=files, **kwargs)
        return res

    def uploadFirmwareOs(self, *args, **kwargs):
        return self.uploadFile(*args, type=settings.FIRMWARE_URL_OS, **kwargs)

    def uploadFirmwareFpga(self, *args, **kwargs):
        return self.uploadFile(*args, type=settings.FIRMWARE_URL_FPGA, **kwargs)


    def reboot(self, *args, **kwargs):
        self.get(*args, uri="/reboot", **kwargs)

    def accessUri(self, *args, **kwargs):
        return self.get(*args, **kwargs)