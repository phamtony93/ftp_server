#################################################################################################
#Author: Tony Pham
#Program: Chat Server
#Course: CS372
#Description: This program acts as server for a chat system. On execution, the program takes
#one parameter as the port number. Clients that connect to this server will take turns sending
#and receiving mesages.
#Last Modified: 2/9/2020
#################################################################################################
import socket
import sys

#getMessage will call recv and wait for client to respond. The response is decoded and returned
#pre: socket is created
#post: function returns decoded message received from socket
def getMessage():
	data = conn.recv(2048)
	return data.decode("utf-8")

#sendMessage will call input and wait for user to provide a response. The response is then encoded and 
#sent to the client. The response is returned. 
#pre: socket is created
#post: function returns reply inputted by the user to be sent to the socket
def sendMessage():
	reply = input('>')
	conn.sendall(reply.encode("utf-8"))
	return reply

#checkForQuit will will True if the input is equal to \quit. This indicates that the connection should be terminated
#pre: string input passed to function
#post: rturns True if input == "\quit" else return False
def checkForQuit(input):
	if input == "\quit":
		return True
	else:
		return False

#Display to user that the connection to the client is terminated
#pre: N/A
#post: terinated connection string displayed to user. Nothing is returned
def terminateConnection():
	print("Connection terminated")

if len(sys.argv) < 2:
    print("Please pass port number as command line argument 1")
    exit(0)

serverHost = 'localhost' #is this right?
serverPort = int(sys.argv[1])



#Title: Socket Programming in Python source code
#Author: Nathan Jennings
#Date: 2/4/2020
#Code Version: Version: 1
#Availability: realpython.com/python-sockets/
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as serverSocket: 
	serverSocket.bind((serverHost, serverPort))
	while True:
		serverSocket.listen()
		conn, addr = serverSocket.accept()
		with conn:
			print('Connected by', addr)
			isDone = False
			while not isDone:
				data = getMessage()	
				if checkForQuit(data):
					terminateConnection()
					isDone = True
				elif data == "-l":
					reply = "Here is my directory"
					conn.sendall(reply)
				elif data == "-g":
					reply = "Let me check if we have that file"
					conn.sendall(reply)
					#reply = sendMessage()
				elif checkForQuit(reply):
					terminateConnection()
					isDone = True
	
