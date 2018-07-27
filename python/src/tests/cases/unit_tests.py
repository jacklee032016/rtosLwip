import os

from tests import settings
from tests import An767TestCase

from cmds.http.HttpClient import WebClient, RestApiClient


class An767UnitTestCases(An767TestCase):

    def setUp(self,  *args, **kwargs):
        super().setUp()
        self.ip = "192.168.166.2"
        self.webClient = WebClient( kwargs.get('debug', True) )


    def tearDown(self):
        super().tearDown()


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
