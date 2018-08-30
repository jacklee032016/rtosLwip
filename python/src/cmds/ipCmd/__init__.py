
import abc
import struct
import logging
import socket
import binascii
import json

from utils import ColorMsg


LOGGER = logging.getLogger(__name__)

MSGLEN = 4096


class CommandCodec(object):
    def __init__(self):
        pass

    def encode(jsonCmd, debug=False):
        """ encapsulate header and tail of IP command """

        # 1: JSON object --> bin data
        bytesCmd = json.dumps(jsonCmd).encode('ascii')
        #bytesCmd = bytes(jsonCmd, "utf-8")
        crc32 = binascii.crc32(bytesCmd) & 0xffffffff

        # 2: pack into packet
        # format '<': little-endian, protect padding at end
        # length of msg in format string
        # length of IP command: include length of CRC
        packedCmd = struct.pack( "<HH%dsI" % len(bytesCmd), 0xA0FA, socket.ntohs(len(bytesCmd) + 4), bytesCmd, socket.htonl(crc32) )
        # ColorMsg.debug_msg("packed data %s" %(packedCmd), debug)
        return packedCmd

    def decode(data, debug=False):
        """
         return JSON object
        """
        # ColorMsg.debug_msg("Received data:'%s'" % (data), debug)
        
        try:
            # 1: unpack json string
            # print("receive from:", binascii.hexlify(data))
            tag, packedLen = struct.unpack("<HH", data[0:4])
            packedLen = socket.ntohs(packedLen)
            # ColorMsg.debug_msg("\nUnpacked TAG 0x%x; length:%d" % (tag, packedLen), debug)
            #msg = data[4:len].decode('utf-8')  #

#            format = b'HH%ds' % (length - 4)
            #print('received %d IP CMD, format:"%s"' % (len, msg))
            (header, _header, jsonBytes) = struct.unpack(b'<HH%ds'%(packedLen-4), data[:packedLen])

            ColorMsg.debug_msg('Received JSON string: "%s":' %(jsonBytes), debug ) # ascii

            try:
                replyJson = json.loads(jsonBytes.decode("ascii"))
                ColorMsg.debug_msg('Received JSON object: "%s":' % (replyJson))
                return replyJson
            except (TypeError, ValueError):
                #raise Exception('Data received was not in JSON format %s'%(jsonBytes))
                ColorMsg.error_msg('Data received was not in JSON format %s'%(jsonBytes))
                return None
            #return json.loads(msg)

            # print(self.replyMsg)
        except (ValueError, TypeError) as e: #KeyError,
            ColorMsg.error_msg("Struct Unpack error: ".format(e))
            # else:
            # remote control is about controlling the model (thrust and attitude)
            # print("Reply msg %s" %(self.replyMsg) )


