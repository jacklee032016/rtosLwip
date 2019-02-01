

SDP UDI:

  curl -H "Content-Type: application/json" -d '{"sdpVideoUri":"12234435.api", "sdpAudioUri":"as12audio.api"}' http://192.168.168.110/service -i


REST API tests
===========================

Reboot:
::

	curl -H "Content-Type: application/json" -d '{"reboot":1}' http://192.168.168.104/service -i


Net Mask read-only:

	curl -H "Content-Type: application/json" -d '{"netmask":"255.255.0.0"}' http://192.168.168.104/service -i

MAC read-only:

	curl -H "Content-Type: application/json" -d '{"MAC":"06:04:25:1c:a0:02"}' http://192.168.168.104/service -i

RS232:

Params-1:

  curl -H "Content-Type: application/json" -d '{"rs232Baudrate":9600, "rs232DataBit":7, "rs232Parity":"odd", "rs232StopBit":1}' http://192.168.168.104/service -i

Params-2:

  curl -H "Content-Type: application/json" -d '{"rs232Baudrate":57600, "rs232DataBit":8, "rs232Parity":"even", "rs232StopBit":2}' http://192.168.168.104/service -i

Wrong params:

  curl -H "Content-Type: application/json" -d '{"rs232Baudrate":576000, "rs232DataBit":5, "rs232Parity":"others", "rs232StopBit":3}' http://192.168.168.104/service -i




Web POST tests
==============================

::

   curl -H "Content-Type: text/html" -d 'rs232Baudrate=57600&rs232DataBit=8&rs232Parity=0&rs232StopBit=1' http://192.168.168.101/rs232Update -i

