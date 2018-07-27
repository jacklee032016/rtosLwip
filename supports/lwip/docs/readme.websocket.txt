							Web Socket Development and Test under LwIP
															Jack Lee, May.18, 2018

curl --include \
     --no-buffer \
     --header "Connection: Upgrade" \
     --header "Upgrade: websocket" \
     --header "Host: example.com:80" \
     --header "Origin: http://example.com:80" \
     --header "Sec-WebSocket-Key: SGVsbG8sIHdvcmxkIQ==" \
     --header "Sec-WebSocket-Version: 13" \
		http://example.com:80/


curl -i -N -H "Connection: Upgrade" -H "Upgrade: websocket" -H "Host: echo.websocket.org" -H "Origin: https://www.websocket.org" https://echo.websocket.org
curl -i -N -H "Connection: Upgrade" -H "Upgrade: websocket" -H "Host: echo.websocket.org" -H "Origin: http://www.websocket.org" http://echo.websocket.org

    Those flags say:
        Return headers in the output
        Don¡¯t buffer the response
        Set a header that this connection needs to upgrade from HTTP to something else
        Set a header that this connection needs to upgrade to a WebSocket connection
        Set a header to define the host (required by later WebSocket standards)
        Set a header to define the origin of the request (required by later WebSocket standards)


