import os

from tests import settings
from tests import An767TestCase

from utils import ColorMsg

from cmds.http.client import WebClient, RestApiClient
from cmds.ipCmd.IpCmd import DeviceIpCmd

class An767UnitTestCases(An767TestCase):

    def setUp(self,  *args, **kwargs):
        super().setUp()
        self.ip = "192.168.166.2"
        self.webClient = WebClient( kwargs.get('debug', True) )
        ColorMsg.debug_msg("setup", debug=True)

    def tearDown(self):
        super().tearDown()
        ColorMsg.debug_msg("teardown", debug=True)


    def _testGetUri(self, uri):
        response = self.webClient.get(ip=self.ip, uri=uri)
        return self.webClient.assertStatusCode(response, 200)

    def testRoot(self):
        self._testGetUri(uri="/")

    def testHtml(self):
        self._testGetUri(uri="/upgradeFpga.html")

    def testCss(self):
        self._testGetUri(uri="/styles.css")

    def testJs(self):
        self._testGetUri(uri="/load_html.js")


class IpCmdsTestCase(An767TestCase):
    def setUp(self,  *args, **kwargs):
        super().setUp()
        self.ip = "192.168.166.2"
        self.debug = True
        self.gateway = "192.168.166.1"
        self.agent = DeviceIpCmd(simGateway=self.gateway, debug=self.debug)
        ColorMsg.debug_msg("setup", debug=True)

    def tearDown(self):
        super().tearDown()
        ColorMsg.debug_msg("tearDown", debug=True)

    def testFind(self):
        nodes = self.agent.find()
        assert  1

    def testRs232(self):
        self.agent.rs232Data(data="112233445566")
        assert 1

