import socket
import pickle
import binascii
import struct
import zlib
import json

def main():
    getParams = '{"cmd":"get_param", "login-ack":"admin", "pwd-msg":"admin", "targ":"FF:FF:FF:FF:FF:FF" }'
    jsonCmd = {"cmd":"get_param", "login-ack":"admin", "pwd-msg":"admin", "targ":"FF:FF:FF:FF:FF:FF" }


    my_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    # Set a timeout so the socket does not block indefinitely when trying to receive data.
    my_socket.settimeout(2.2)
    my_socket.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST,1)


    try:
        message = "This is a test"# Send data to the multicast group
        # sent = my_socket.sendto(message, multicast_group)
        # sent = my_socket.sendto("wearetherock", ('<broadcast>', 3600))
        binObj = pickle.dumps(getParams)
        #paramStr = getParams.encode('utf-8')
        paramStr = json.dumps(jsonCmd).encode('ascii')
        #binObj = ' '.join(format(ord(x), 'b') for x in getParams)
        crc32 = binascii.crc32(paramStr)
        print("crc232: %d\n" % crc32)
        print('sending %d bytes: "%s"\n' % (len(paramStr), paramStr))

        # pkt = binObj + crc32
        values = (0xa0fa, socket.ntohs(len(paramStr)+4), paramStr, socket.htonl(crc32))
        print('values ', values)

        # format '<' protect padding at end
        s = struct.Struct(b'<H H %ds I' %(len(paramStr)) )
        packed_data = s.pack(*values)
        print('packed values \n', packed_data)
        my_socket.bind(("192.168.166.1", 3600))
        sent = my_socket.sendto(packed_data,  ('<broadcast>', 3600))

        # Look for responses from all recipients
        while True:
            print('waiting to receive')
            try:
                data, server = my_socket.recvfrom(2048)
                print('received length %d bytes from %s: :"%s"' % (len(data), server, data ) )
                s2 = struct.Struct(b'H H' )
                (header, length) = struct.unpack(b'H H', data[:4])
                length = socket.ntohs(length)
                format=b'HH%ds'%(length-4)
                print('received %d IP CMD, format:"%s"' %(length, format) )

                (header, _header ,decoded_bytes) = struct.unpack(format, data[:length])
                print('received %s ' %(decoded_bytes.decode("ascii") ) )
                try:
                    deserializedDict = json.loads(decoded_bytes.decode("ascii"))
                    print(deserializedDict)
                except (TypeError, ValueError):
                    raise Exception('Data received was not in JSON format')
                    #result = pickle.loads(data)
            except socket.timeout:
                print("timed out, no more responses")
                break
            else:
                print('OK'  )

    finally:
        print('closing socket')
        my_socket.close()

    #my_socket.close()
    #client.bind(("", 37020))
    #while True:
    #	data, addr = sock.recvfrom(1024)

    #	print("received message: %s"%data)

if __name__ == "__main__" :
    main()

