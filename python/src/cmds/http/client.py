"""
HTTP Client, based on requests, and provide interface to HTML and Rest API access
"""

import json
import time
import requests

from base64 import b64encode

from utils import ColorMsg

import copy
import re


class HttpClient(object):

    def __init__(self, *args, **kwargs):
        self.client = requests.Session()
        self.debug = kwargs.get("debug", False)
        self.totalTests = 0

    def _html_data(self, kwargs):
        if 'data' in kwargs:
            #kwargs['data']['csrf_token'] = self.csrf_token
            pass
        #if not kwargs.get('content_type'):
        #    kwargs['content_type'] = 'application/x-www-form-urlencoded'
        return kwargs

    def _json_data(self, kwargs, csrf_enabled=True):
        if 'data' in kwargs:
            kwargs['data'] = json.dumps(kwargs['data'])
        #if not kwargs.get('content_type'):
        #    kwargs['content_type'] = 'application/json'
        if 'auth' in kwargs:
            if type(kwargs['auth']) is tuple:
                username = kwargs['auth'][0]
                password = kwargs['auth'][1]
                kwargs['headers'] = {'Authorization': 'Basic %s' % b64encode(bytes(username + ':' + password, "utf-8")).decode("ascii")}
                kwargs.pop('auth')
            else:
                token = kwargs['auth']
                kwargs['headers'] = {'Authorization': 'Token {0}'.format(token)}
                kwargs.pop('auth')
        return kwargs

    def createUrl(self, *args, **kwargs):
        pass

    def parseResponse(self, resp, *args, **kwargs):
        if self.contentLength == 0:
            ColorMsg.error_msg("\n\tURI %s: status: failed in request\n"%(kwargs.get("uri")) )
        print("headers:" + str(resp.headers))
        # print("******************")
        print("\nNo.%d Test: \tURI %s; Code: %s Type: %s; Res Size: %s; Update Size:%s; Time: %f ms\n" \
              %(self.totalTests,  kwargs.get("uri"),str(resp.status_code),  resp.headers['Content-Type'], self.contentLength, self.number, self.timeused) )
        return resp

    #@remove_args
    def _request(self, method, *args, **kwargs):
        #kwargs.setdefault('content_type', 'application/json')
        ColorMsg.debug_msg("in request, kwargs ID:%d %s" % (id(kwargs), kwargs), self.debug)
        self.createUrl(self, *args, **kwargs)

        ColorMsg.debug_msg("METHOD \"%s\" on URL %s"%(method.__name__, self.url), self.debug)
        #print("kwargs %s"%(kwargs) )
        # kwargs.setdefault('follow_redirects', True)

        kwargs2 = copy.copy(kwargs)

        response = None
        done = False

        while not done:
            try:
                start_time = time.time()  # time() is float
                response = method(self.url, **kwargs2)
                self.timeused = (time.time() - start_time)*1000 # millsecond
                ColorMsg.debug_msg("Response after %s ms" %(self.timeused) , self.debug)
                self.totalTests = self.totalTests+1
                # print("passed kwargs %s" % (kwargs2))
                done = True
            except TypeError as err:  # for: TypeError: request() got an unexpected keyword argument 'uri'
                ColorMsg.debug_msg("TypeError output:{0}".format(err), self.debug)
                # re.findall() return a list, so get first item in list
                key = re.findall("\'(\w+)\'", format(err))[0]
                ColorMsg.debug_msg("Method \"%s\" removing \"%s\"" % (method.__name__, str(key)), self.debug )
                del kwargs2[key]  # Remove offending key

        #response = method(self.url, *args, **kwargs)
        if response is None:
            self.contentLength = 0
            return
        self.contentLength = response.headers['Content-Length']
        return self.parseResponse(response, *args, **kwargs)

    def _jrequest(self, *args, **kwargs):
        return self._request(*args, **kwargs)

    def get(self,*args, **kwargs):
        return self._request(self.client.get, *args, **self._html_data(kwargs) )

    def post(self, *args, **kwargs):
        return self._request(self.client.post, *args, **self._html_data(kwargs))

    def put(self, *args, **kwargs):
        return self._request(self.client.put, *args, **self._html_data(kwargs))

    def delete(self, *args, **kwargs):
        return self._request(self.client.delete, *args, **kwargs)

    def jget(self, *args, **kwargs):
        return self._jrequest(self.client.get, *args, **kwargs)

    def jpost(self, *args, **kwargs):
        return self._jrequest(self.client.post, *args, **self._json_data(kwargs))

    def jput(self, *args, **kwargs):
        return self._jrequest(self.client.put, *args, **self._json_data(kwargs))

    def jdelete(self, *args, **kwargs):
        return self._jrequest(self.client.delete, *args, **kwargs)

    def getCookies(self, response):
        #cookies = {}
        #for value in response.headers.get_all("Set-Cookie"):
            #cookies.update(parse_cookie(value))
        return response.cookies

    def assertEquals(self, var1, var2):
        if var1 == var2:
            return True
        else:
            return False

    def assertIn(self, var, array):
        if var in array:
            return True
        else:
            return False


    def assertStatusCode(self, response, status_code):
        """Assert the status code of a Flask test client response

        :param response: The test client response object
        :param status_code: The expected status code
        """
        self.assertEquals(status_code, response.status_code)
        return response

    def assert_ok(self, response):
        """Test that response status code is 200

        :param response: The test client response object
        """
        return self.assertStatusCode(response, 200)

    def assertBadRequest(self, response):
        """Test that response status code is 400

        :param response: The test client response object
        """
        return self.assertStatusCode(response, 400)

    def assertForbidden(self, response):
        """Test that response status code is 403

        :param response: The test client response object
        """
        return self.assertStatusCode(response, 403)

    def assertNotFound(self, response):
        """Test that response status code is 404

        :param response: The test client response object
        """
        return self.assertStatusCode(response, 404)

    def assertContentType(self, response, content_type):
        """Assert the content-type of a Flask test client response

        :param response: The test client response object
        :param content_type: The expected content type
        """
        return self.assertEquals(content_type, response.headers['Content-Type'])
        #return response

    def assert_okHtml(self, response):
        """Assert the response status code is 200 and an HTML response

        :param response: The test client response object
        """
        return self.assert_ok(
            self.assertContentType(response, 'text/html; charset=utf-8'))

    def assertJson(self, response):
        """Test that content returned is in JSON format

        :param response: The test client response object
        """
        return self.assertContentType(response, 'application/json')

    def assert_okJson(self, response):
        """Assert the response status code is 200 and a JSON response

        :param response: The test client response object
        """
        return self.assert_ok(self.assertJson(response))

    def assertBadJson(self, response):
        """Assert the response status code is 400 and a JSON response

        :param response: The test client response object
        """
        return self.assertBadRequest(self.assertJson(response))

    def assertCookie(self, response, name):
        """Assert the response contains a cookie with the specified name

        :param response: The test client response object
        :param key: The cookie name
        :param value: The value of the cookie
        """
        self.assertIn(name, self.getCookies(response))

    def assertCookieEquals(self, response, name, value):
        """Assert the response contains a cookie with the specified value

        :param response: The test client response object
        :param name: The cookie name
        :param value: The value of the cookie
        """
        self.assertEquals(value, self.getCookies(response).get(name, None))


class WebClient(HttpClient):

    def __init__(self, *args, **kwargs):
        super().__init__( *args, **kwargs)

    def createUrl(self, *args, **kwargs):
        self.url = "http://"+ kwargs.get("ip", "192.168.168.120")+":"+str(kwargs.get("port", 80))+kwargs.get("uri", "/")
        # print("%s is called"%(self.__class__.__name__) )

    def uploadFile(self, *args, **kwargs):
        files = {'file': open(kwargs.get("file", "testHttp.py"), 'rb')}
        res = self.post(*args, files=files, **kwargs)

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
