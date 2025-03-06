# echo-client.py : Test client

import socket
import sys
import argparse

parser = argparse.ArgumentParser(description='Tracker - Echo Test Client')

parser.add_argument('host', type=str, help='Hostname or IP address of the server')
parser.add_argument('port', type=int, help='The port number for the server')

args = parser.parse_args()

# Create a UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

server_address = (args.host, args.port)

message = 'This is the message.  It will be repeated.'

try:
   print('Sending to: ' + str(server_address))
   sent = sock.sendto(message.encode(), server_address)
   print('  Sent ' + str(sent) + ' bytes')

   print('  Awaiting data back via a recv call')
   data, server = sock.recvfrom(4096)

   print('Server: ' + str(server))
   print('Data Length: ' + len(data))


finally:
    sock.close()