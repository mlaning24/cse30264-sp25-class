# echo-client.py : Test client
#
# Make N echo requests to the server that will respond appropriately with Echo Response
# messages

import socket
import sys
import argparse
import time
import random

def doEcho (theSocket, theServerInfo):
   try:
      theBytes = bytearray()

      # The type is 5
      theBytes.append(5)
      # The length is 4 across two bytes (16 bits)
      theBytes.append(0)
      theBytes.append(4)
      # Generate a nonce (random value) between 1 and 100 million
      theRandValue = random.randint(1, 100000000)
      print('  Generated Nonce: ' + str(theRandValue))

      # Put this in big endian order and append it
      nonceArray = theRandValue.to_bytes(4, byteorder='big')
      nonceLittle = theRandValue.to_bytes(4, byteorder='little')
      for theByte in nonceArray:
         theBytes.append(theByte)

      # Remember - almost all protocols will be big endian - the little endian
      # is just to show how little endian works in contrast to big endian
      print('   Nonce as Byte Array (Endian = Big):     ' + ' '.join([f'{byte:02x}' for byte in nonceArray]))
      print('   Nonce as Byte Array (Endian = Little):  ' + ' '.join([f'{byte:02x}' for byte in nonceLittle]))

      print('Sending ' + str(len(theBytes)) + ' bytes to ' + str(theServerInfo))
      print('  Hex Representation: ' + ' '.join([f'{byte:02x}' for byte in theBytes]))
      sent = sock.sendto(theBytes, theServerInfo)

      print('  Awaiting data back via a recv call')
      data, server = sock.recvfrom(4096)

      print('From Server:  ' + str(server))
      print(' Data Length: ' + str(len(data)))
      # This should be a binary array of data (e.g. start with b)
      print(' Data: ' + ' '.join([f'{byte:02x}' for byte in data]))
   except Exception as e:
      print(f"Error: {e}")
   else:
      print('Success - no errors detected!!')

   return True

parser = argparse.ArgumentParser(description='Tracker - Echo Test Client')

parser.add_argument('host', type=str, help='Hostname or IP address of the server')
parser.add_argument('port', type=int, help='The port number for the server')
parser.add_argument('--count', type=int, help='Change the number of times to do the echo', default=2)
parser.add_argument('--delay', type=float, help='Delay in seconds between echo requests', default=1.0)

args = parser.parse_args()

# Create a UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# The server information is a tuple containing the IP address (or hostname) and
# the port number
server_address = (args.host, args.port)

# Echo for the specified count
for theCount in range(args.count):
   print('== Echo Sequence ==')
   # Make an echo request to the server (can be C++ or Python)
   doEcho(sock, server_address)
   # Sleep for the requested delay
   time.sleep(args.delay)
   print('** End Echo Requence **')

sock.close()
