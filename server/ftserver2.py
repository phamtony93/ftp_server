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
import os

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

#Retrieve list of files based on path and extension provided
#ore: N/A
#post: returns a string of file names
def retrieveFiles(path, extension=False):
	fileList = ""
	if extension:
		for filename in os.listdir(path):
			if filename.endswith(extension):
				fileList += filename
				fileList += '\n'
	else:
		for filename in os.listdir(path):
			fileList += filename
			fileList += '\n'
	return fileList

#Searches through directory for file name. Returns true if found
#pre: N/A
#post: returns true if file found, false otherwise
def searchForFile(path, file):
	for filename in os.listdir(path):
		if filename == file:
			return True
	return False





if len(sys.argv) < 2:
    print("Please pass port number as command line argument 1")
    exit(0)

serverHost = 'localhost' #is this right?
serverPort = int(sys.argv[1])


#temp = retrieveFiles('.', '.txt');
#print(temp)
#exit(0)
########read list of files
#reply = ""
#for file in os.listdir('.'):
#	if file.endswith(".txt"):
#		reply += file
#		reply += '\n'
#print(reply)
#exit(0)

######file exists
#fileName = "small.txt"
#for file in os.listdir('.'):
#	if file == fileName:
#		found = 1
#	else:
#		found = 0
#if found == 1:
#	f = open(fileName, "r")
#print(f.read())
#exit(0)

#Title: Socket Programming in Python source code
#Author: Nathan Jennings
#Date: 2/4/2020
#Code Version: Version: 1
#Availability: realpython.com/python-sockets/
print('Server open on ' + str(serverPort))
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as serverSocket: 
	serverSocket.bind((serverHost, serverPort))
	while True:
		serverSocket.listen()
		conn, addr = serverSocket.accept()
		with conn:
			print('Connected by', addr)
			isDone = False
			data = getMessage()
			print('Sent from client: ' + data);
			data = data.split(' ')
			command = data[0]
			text = data[1]
			serverPortData = int(data[-1])
			#Validate response
			if data[0] == "-l" or data[0] == "-g":
				reply = ("Establishing data connection on port " + str(serverPortData))
				conn.sendall(reply.encode("utf-8"))
				
				serverPortData = data[-1];
				with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as serverSocketData:
					serverSocketData.bind((serverHost, int(serverPortData)))
					serverSocketData.listen()
					connData, addrData = serverSocketData.accept()
					with connData:
						if data[0] == "-l":
							print("List directory requested on port " + str(serverPortData))
							reply = retrieveFiles('.', '.txt')
							#print("str length is: " + str(len(reply)))
							#connData.sendall(str(len(reply)).encode("utf-8"))
							connData.sendall(reply.encode("utf-8"))
							connData.close()
#							data = connData.recv(2048)
						else:
							if searchForFile('.', data[1]):
								inputFile = open(data[1], "r")
								reply = inputFile.read()
#								reply = "file found\n"
								connData.sendall(reply.encode("utf-8"))
								connData.close()
#								data = connData.recv(2048)
							else:
								reply = "File Not Found"
								connData.sendall(reply.encode("utf-8"))
								connData.close()
#								data = connData.recv(2048)				
#						print('here')
#						reply = ("sending data now through new connection")
#						connData.sendall(reply.encode("utf-8"))
#						data = connData.recv(2048)



			else:
				print("invalid")
#			while not isDone:
#				data = getMessage()	
#				print("message is" + data)
#				if checkForQuit(data):
#					terminateConnection()
#					isDone = True
#				elif data == "-l":
#					reply = "Here is my directory"
#					conn.sendall(reply.encode("utf-8"))
#				elif data == "-g":
#					reply = "Let me check if we have that file"
#					conn.sendall(reply.encode("utf-8"))
#					#reply = sendMessage()
##				elif checkForQuit(reply):
##					terminateConnection()
##					isDone = True
#				else:
#					reply = "Command not recognized"
#					conn.sendall(reply.encode("utf-8"))
	
