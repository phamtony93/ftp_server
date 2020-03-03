import socket
import sys

clientHost = 'localhost'
clientPort = int(sys.argv[1])

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as clientSocket:
    clientSocket.connect((clientHost, clientPort))
    clientSocket.sendall(b'Hello, world')
    data = clientSocket.recv(2048)

print('received', repr(data))