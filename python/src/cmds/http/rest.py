
import json

from.client import HttpClient

from utils import ColorMsg

class RestApiClient(HttpClient):

    def __init__(self, *args, **kwargs):
        super().__init__(self, *args, **kwargs)

    def createUrl(self, *args, **kwargs):
        self.url = "http://"+ kwargs.get("ip", "192.168.168.120")+":"+str(kwargs.get("port", 80))+kwargs.get("uri", "/x-nmos/self")
        super().createUrl(self, *args, **kwargs)

    def parseResponse(self, response, *args, **kwargs):
        if self.assertContentType(response, "application/json"):
            ColorMsg.error_msg("Content type '%s' is not validate for REST API" %(response.headers['Content-Type']) )
            return response

        try:
            jData = json.loads(response.content.decode("utf-8"))

            print("The response contains {0} properties".format(len(jData)))
            if (response.ok):
                print('raw "%s"\n' % (json.dumps(jData, indent=4, sort_keys=True)))
            else:
                print("\n")
            for key in jData:
                print(key + " : " + str(jData[key]))
        except (TypeError, ValueError):
            raise Exception("parse JSON string failed \"%s\"" % (resStr))
