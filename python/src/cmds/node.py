
from collections import namedtuple

# Specify the Employee namedtuple.


def createNode(data):
    Node = namedtuple("Node", data.keys())
    node = Node(*data.values())

    print(node)
    return node

class Node(object):

    def __init__(self, dictData):
        """Constructor"""
        for key in dictData.keys():
            setattr(self, key, dictData[key])

    # ----------------------------------------------------------------------
    def __repr__(self):
        """"""
        return "<Node: %s>" % self.__dict__

    def __str__(self):
        return '''
    Node < %s: %s: %s, %s: DHCP:%d, %s(%s, %s): %s>
        RS232 <bps:%d, data:%d, stop:%d, parity:%s>
        Network <isDIP:%d, isMCast:%d, IP:%s, Port:%d:%d:%d:%d>
        Video <%dx%d; %s, fps:%d, depth:%d, isSegment:%d, isInterlaced:%d >
        Audio <channel:%d, Freq:%d, Bits:%d >'''\
               %(self.cName,self.model, self.pName, self.fwVer, self.isDhcp, self.ip, self.mask, self.gateway, self.mac,
                 self.RS232Baudrate, self.RS232Databits, self.RS232Stopbits, self.RS232Parity,
                 self.isDipOn, self.isMCAST, self.MCASTip, self.vidPort, self.audPort, self.datPort, self.strPort,
                 self.vidH, self.vidW, self.vidClrSpace, self.vidFps, self.vidDepth, self.vidIsSgmt, self.vidIsIntlc,
                 self.audChan, self.audFrq, self.audBit
                 )

