import iperf3

client = iperf3.Client()
client.duration = 1
#client.server_hostname = '192.168.166.2'
client.server_hostname = '127.0.0.1'
#client.port = 5201
#client.port = 5001
client.verbose = True
result = client.run()
print(result) #.sent_Mbps
