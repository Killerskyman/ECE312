#!/usr/bin/env python3
# From: https://realpython.com/python-sockets/#echo-client-and-server

import socket
import sys

HOST = '127.0.0.1'  # The server's hostname or IP address
PORT = 65432        # The port used by the server
msg  = b'Hello, world'

if len(sys.argv) == 3:
    HOST = sys.argv[1]
    PORT = int(sys.argv[2])
    
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    print('Sending: ', msg)
    s.sendall(msg)
    data = s.recv(1024)

print('Received', repr(data))