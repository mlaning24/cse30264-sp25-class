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

while True:
   try:
      print('  Waiting for a message from the client')
      data, clientInfo = sock.recvfrom(4096)

      print('  Received a ' + str(len(data)) + ' byte message from host ' + str(clientInfo))
      print('  Message Dump: ' + str(data))

      # Send a response back

      theResponse = bytearray()
      theResponse.append(6)

      # The response length is 13 bytes
      #  Status Code for the Command (0 = No Errors (Success), Non-Zero is an Error)
      #  Reflection of the nonce (32 bits)
      #  High Resolution Time (timeval)
      #     Time in Seconds (32 bits)
      #     Fractional Time in Microseconds (32 bits)
      theResponse.append(0)
      theResponse.append(13)

      if len(data) != 7:
         theResponse.append(1)
      else:
         theResponse.append(0)

      # If there is a nonce - reflect it back, otherwise reflect a zero
      if len(data) >= 7:
         # Get the nonce (32 bit random value) for reflection
         for theNonceByte in range(4):
            theResponse.append(data[3+theNonceByte])
      else:
         for theNonceByte in range(4):
            theResponse.append(0)

      # Get the current high resolution time (seconds, microseconds)
      for theNonceByte in range(8):
         theResponse.append(0)

      # Send the response
      sent = sock.sendto(theResponse, clientInfo)

   except Exception as e:
      print(f"Error: {e}")
