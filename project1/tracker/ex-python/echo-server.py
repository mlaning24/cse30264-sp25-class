# echo-server.py : Reduced version of the tracking server doing only the echo response

import socket
import sys
import argparse
import time

parser = argparse.ArgumentParser(description='Tracker - Server - Echo Only')

parser.add_argument('port', type=int, help='The port number for the server')

args = parser.parse_args()

# Create a UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

print('Binding to port ' + str(args.port))

server_address = ('', args.port)
sock.bind(server_address)
print('  Success!')

message = 'This is the message.  It will be repeated.'

while True:
   try:
      print('  Waiting for a message from the client')
      data, clientInfo = sock.recvfrom(4096)

      print('  Received a ' + str(len(data)) + ' byte message from host ' + str(clientInfo))
      print('  Message Dump: ' + str(data))

      # Send a response back
      sent = sock.sendto('Success!'.encode(), server_address)

      time.sleep(1)

   except Exception as e:
      print(f"Error: {e}")
