from tests import An767TestCase

from cmds.ipCmd.IpCmdCtrl import DeviceIpCmd
from cmds.http.web import WebClient
from tests import An767TestCase

from utils import ColorMsg
from utils import settings

class HttpUnitTestCases(An767TestCase):

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

    def testAllStatic(self):
        """
        This case can be used as performance testing for OOM of TCP_PCB
        """
        for page in settings.STATIC_PAGES:
            self._testGetUri(uri=page)


class IpCmdsTestCase(An767TestCase):
    def setUp(self,  *args, **kwargs):
        """
        called per test case, eg. every test method in this class
        :param args:
        :param kwargs:
        :return:
        """
        super().setUp()
        self.ip = "192.168.166.2"
        self.debug = True
        self.gateway = "192.168.166.1"
        self.agent = DeviceIpCmd(simGateway=self.gateway, debug=self.debug)
        #self.agent = DeviceIpCmd( debug=self.debug)
        ColorMsg.debug_msg("setup", debug=True)

    def tearDown(self):
        """
        called in every test case, eg. every test method in this class
        :return:
        """
        super().tearDown()
        ColorMsg.debug_msg("tearDown", debug=True)

    def testFind(self):
        """
        2 commands finished in one socket interface
        :return:
        """

        #nodes = self.agent.find()
        self.agent.rs232Data(data="112233445566")

        assert  1

    #def testRs232(self):
    #    assert 1

