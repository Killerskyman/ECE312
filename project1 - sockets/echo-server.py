#!/usr/bin/env python3
# From: https://realpython.com/python-sockets/#echo-client-and-server

import socket
import sys

HOST = '127.0.0.1'  # Standard loopback interface address (localhost)
PORT = 65432        # Port to listen on (non-privileged ports are > 1023)

# print(sys.argv)
if len(sys.argv) == 2:
    PORT = int(sys.argv[1])

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    # '' means accept connections from other machines
    s.bind(('', PORT))
    s.listen()
    conn, addr = s.accept()
    with conn:
        print('Connected by', addr)
        while True:
            data = conn.recv(1024)
            if not data:
                break
            print('Received', repr(data))
            conn.sendall(data)