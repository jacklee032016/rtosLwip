

May,29, 2018
	test API

curl -H "Content-Type: application/json" -X GET http://192.168.166.3/x-nmos/connection/v1.0/ -v  | python -mjson.tool

curl -H "Content-Type: application/json" -X GET http://192.168.166.3/x-nmos/node/v1.2/devices -v 

curl -H "Content-Type: application/json" -X GET http://192.168.166.3/x-nmos/node/v1.2/self -v  | python -mjson.tool

curl -H "Content-Type: application/json" -X GET http://192.168.166.3/x-nmos/node/v1.2/ -v | python -mjson.tool



curl -X GET  -d '{"username":"xyz","password":"xyz"}' http://192.168.166.3 -v

curl -H "Content-Type: application/json" -X GET http://192.168.166.3/x-nmos/node/v1.2/self/single/123e4567-e89b-12d3-a456-42665544abcd -v

curl -H "Content-Type: application/json" -X GET http://192.168.166.3/x-nmos/node/v1.2/ -v


curl -H "Content-Type: application/json" POST   -d '{"username":"xyz","password":"xyz"}' http://192.168.166.3/x-nmos/node/v1.2/ -v

curl -H 'Content-Type: application/json' -X PUT -d '{"tags":["tag1","tag2"],"question":"Which band?","answers":[{"id":"a0","answer":"Answer1"},{"id":"a1","answer":"answer2"}]}' http://192.168.166.3/x-nmos/node/v1.2/ -v

PUT /x-nmos/node/v1.2/ HTTP/1.1
Host: 192.168.166.3
User-Agent: curl/7.51.0
Accept: */*
Content-Type: application/json
Content-Length: 123

{"tags":["tag1","tag2"],"question":"Which band?","answers":[{"id":"a0","answer":"Answer1"},{"id":"a1","answer":"answer2"}]}'

PATCH
curl --request PATCH http://192.168.166.3/x-nmos/node/v1.2/status=closed -v

Received 109 bytes
[src/mux/http/muxHttpMain.c-198.__mhttpRecv()]: recv:'PATCH /x-nmos/node/v1.2/status=closed HTTP/1.1
Host: 192.168.166.3
User-Agent: curl/7.51.0
Accept: */*

'

curl --data '{"field": "new_value"}' -X PATCH http://192.168.166.3/x-nmos/node/v1.2/ -v

recv:'PATCH /x-nmos/node/v1.2/ HTTP/1.1
Host: 192.168.166.3
User-Agent: curl/7.51.0
Accept: */*
Content-Length: 22
Content-Type: application/x-www-form-urlencoded

{"field": "new_value"}'


DELETE
curl -X "DELETE" http://192.168.166.3/x-nmos/node/v1.2/ -v
Received 97 bytes
[src/mux/http/muxHttpMain.c-198.__mhttpRecv()]: recv:'DELETE /x-nmos/node/v1.2/ HTTP/1.1
Host: 192.168.166.3
User-Agent: curl/7.51.0
Accept: */*

'
curl -X DELETE "https://myhost/context/path/users/$(echo -ne "OXYugGKg207g5uN/07V" | xxd -plain | tr -d '\n' | sed 's/\(..\)/%\1/g')"


[root@localhost lwip]# curl -H "Content-Type: application/json" -X GET -d '{"username":"xyz","password":"xyz"}' http://www.w3.org/ -v
*   Trying 128.30.52.100...
* TCP_NODELAY set
* Connected to www.w3.org (128.30.52.100) port 80 (#0)
> GET / HTTP/1.1
> Host: www.w3.org
> User-Agent: curl/7.51.0
> Accept: */*
> Content-Type: application/json
> Content-Length: 35
> 
* upload completely sent off: 35 out of 35 bytes
< HTTP/1.1 200 OK
< Date: Thu, 17 May 2018 18:38:56 GMT
< Content-Location: Home.html
< Vary: negotiate,accept,Accept-Encoding,upgrade-insecure-requests
< TCN: choice
< Last-Modified: Thu, 17 May 2018 09:16:48 GMT
< ETag: "a4fb-56c634b832000;89-3f26bd17a2f00"
< Accept-Ranges: bytes
< Content-Length: 42235
< Cache-Control: max-age=600
< Expires: Thu, 17 May 2018 18:48:56 GMT
< P3P: policyref="http://www.w3.org/2014/08/p3p.xml"
< Content-Type: text/html; charset=utf-8
< 


curl -H "Content-Type: application/json" -X GET -d '{"username":"xyz","password":"xyz"}' http://localhost:12000/x-nmos/node/v1.2/ -v
*   Trying 127.0.0.1...
* TCP_NODELAY set
* Connected to localhost (127.0.0.1) port 12000 (#0)
> GET /x-nmos/node/v1.2/ HTTP/1.1
> Host: localhost:12000
> User-Agent: curl/7.51.0
> Accept: */*
> Content-Type: application/json
> Content-Length: 35
> 
* upload completely sent off: 35 out of 35 bytes
< HTTP/1.1 200 OK
< Access-Control-Allow-Origin: *
< Access-Control-Expose-Headers: Content-Length
< Content-Length: 64
< Content-Type: application/json
< 
* Curl_http_done: called premature == 0
* Connection #0 to host localhost left intact
["self/","devices/","sources/","flows/","senders/","receivers/"]


curl -H "Content-Type: application/json" -X GET http://192.168.166.3/ -v

GET:
curl -H "Content-Type: application/json" -X GET http://192.168.166.3/x-nmos/ -v
> GET /x-nmos/ HTTP/1.1
> Host: 192.168.166.3
> User-Agent: curl/7.51.0
> Accept: */*
> Content-Type: application/json
> 
* HTTP 1.0, assume close after body
< HTTP/1.0 404 File not found
< Server: MuxLab/500767
< Content-Length: 565
< Content-type: text/html
< 
<html>
<head><title>lwIP - A Lightweight TCP/IP Stack</title></head>
<body bgcolor="white" text="black">

    <table width="100%">
      <tr valign="top"><td width="80">	  
	  <a href="http://www.sics.se/"><img src="/img/sics.gif"
	  border="0" alt="SICS logo" title="SICS logo"></a>
	</td><td width="500">	  
	  <h1>lwIP - A Lightweight TCP/IP Stack</h1>
	  <h2>404 - Page not found</h2>
	  <p>
	    Sorry, the page you are requesting was not found on this
	    server. 
	  </p>
	</td><td>
	  &nbsp;
	</td></tr>
      </table>
</body>
</html>


POST
curl -d '{"key1":"value1", "key2":"value2"}' -H "Content-Type: application/json" POST http://192.168.166.2/x-nmos/node -v
 POST /x-nmos/node HTTP/1.1
> Host: 192.168.166.3
> User-Agent: curl/7.51.0
> Accept: */*
> Content-Type: application/json
> Content-Length: 34
> 
* upload completely sent off: 34 out of 34 bytes
* Curl_http_done: called premature == 0
* Empty reply from server
* Connection #1 to host 192.168.166.3 left intact
curl: (52) Empty reply from server



curl -d '{"key1":"value1", "key2":"value2"}' -H "Content-Type: application/json" -X PUT http://192.168.166.3/x-nmos/node -v
*   Trying 192.168.166.3...
* TCP_NODELAY set
* Connected to 192.168.166.3 (192.168.166.3) port 80 (#0)
> PUT /x-nmos/node HTTP/1.1
> Host: 192.168.166.3
> User-Agent: curl/7.51.0
> Accept: */*
> Content-Type: application/json
> Content-Length: 34
> 
* upload completely sent off: 34 out of 34 bytes
* Curl_http_done: called premature == 0
* Empty reply from server
* Connection #0 to host 192.168.166.3 left intact
curl: (52) Empty reply from server
