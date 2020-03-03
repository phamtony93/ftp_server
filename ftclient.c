/***********************************************************************************************
 * Author: Tony
 * Program: Chat Client
 * Description: This program allows a user to connect to a chat server. On execution, this 
 * program takes 2 parameters, host and port number.
 * Course: CS372 Intro to Networks
 * Last Modified: 2/9/2020
************************************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

/************************************************************************************
 * This function prompts the user for a handle and appends '>' to it.
 * pre: pointer to char buffer and size of buffer passed to function
 * post: buffer is set to null terminators and then user input is inserterd.
 * getchar() is called to clear the newline and an '>' char is appended to the handle.
 * nothing is returned.
*************************************************************************************/
getHandleFromUser(char* buffer) {
	memset(buffer, '\0', strlen(buffer));
	printf("Enter your user handle: ");
	scanf("%s", buffer);
	getchar();
	fflush(stdin);
	buffer[strlen(buffer)] = '>';
}

/*
initServerStruct(struct sockaddr_ini *serverAddress, char*argv[]) {
	memset((char*)serverAddress, '\0', sizeof(*serverAddress));
	serverAddress->sin_family = AF_INET;
	serverAddress->sin_port = htons(atoi(argv[2]));
	memcpy((char*)serverAddress->sin_addr.s_addr
*/

/************************************************************************************************
 * SendMessage takes in a socket, a message and the size of the message and will send the message
 * to the socket with the send(). If message is not sent or if less than the message size is sent, 
 * an error message will be displayed.
 * pre: pointer to char messag, size of mesage, and an int socket is passed to the function
 * post: message is sent to the socket. Nothing is returned.
************************************************************************************************/
sendMessage(char* message, int socketFD, int size) {
	int charsWritten;
	charsWritten = send(socketFD, message, size, 0);
	if (charsWritten < 0) {
		fprintf(stderr, "CLIENT: ERROR Writing to socket\n");
	};
	if (charsWritten < strlen(message)) {
		fprintf(stderr, "CLIENT: WARNING: Not all data written to socket!\n");
	};
}

/***********************************************************************************************
 * readMessage takes in a socket, a pointer to char as a buffer, and the size of the buffer.
 * The function will read a message into the buffer from the socket. If no data was read, an
 * error message will be displayed.
 * pre: pointer to char buffer, size of buffer, and int socket is passed to function
 * post: message from socket is read into the buffer. Nothing is returned
***********************************************************************************************/
readMessage(char* buffer, int socketFD, int size) {
	int charsRead;
	memset(buffer, '\0', size);
	charsRead = recv(socketFD, buffer, size, 0);
	if (charsRead < 0) {
		fprintf(stderr, "CLIENT: ERROR reading from socket");
	};
};

/***********************************************************************************************
 * checkForQuit takes in a pointer to char and will determine if the buffer is equal to '\quit'
 * pre: pointer to char is sent to function
 * post: if content in buffer == '\quit' then return 1, else return 0
***********************************************************************************************/
int checkForQuit(char* buffer) {
	if (strcmp(buffer, "\\quit") == 0) {
		return 1;
	} else {
		return 0;
	};
};

/************************************************************************************************
 * createSocket will return a socket with a TCP connection.
 * pre: N/A
 * post: returns a TCP socket 
************************************************************************************************/
int createSocket() {
	int socketFD;
	socketFD = socket(AF_INET, SOCK_STREAM, 0);
	if (socketFD < 0) fprintf(stderr, "CLIENT: ERROR CONNECTING\n");
	return socketFD;
};

/*****************************************************************************************************
 * connectSocket takes in a socket, a struct sockaddr_in and the size of the sockddr_in. The function
 * pre: sockaddr_in is initialized and passed to connectSocket
 * post: connects socket to sockaddr_in
******************************************************************************************************/
//will establish connection between the socket and the address.
connectSocket(struct sockaddr_in serverAddress, int socketFD, int size) {
	if (connect(socketFD, (struct sockaddr*)&serverAddress, size) < 0) {
		fprintf(stderr, "CLIENT: ERROR CONNECTING\n");
	};
};

/*****************************************************************************************************
 * output that connection is terimnated and set the isDone flag to true.
 * pre: isDone flag initialized and passed to terminateConnection
 * post isDone set to 1 and termination message is displayed. Nothing is returned
*****************************************************************************************************/
terminateConnection(int *isDone) {
	printf("Connection terminated\n");
	*isDone = 1;
};

int main(int argc, char*argv[]) {
	int socketFD, portNumber, charsRead;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char buffer[100000];
	char userHandle[1000];
	char greeting[11] = "hello world";
	int isDone = 0;

	//Get handle from user
	getHandleFromUser(userHandle);

	//set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); //Clear out the address struct
	portNumber = atoi(argv[2]); //Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; //Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); //Convert port number to network type
	serverHostInfo = gethostbyname(argv[1]);	//convert the machine name into a special form of address
	if (serverHostInfo == NULL) {
		fprintf(stderr, "CLIENT: ERROR, no such host\n");
	};
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length);

	//Set up the socket
	//socketFD = socket(AF_INET, SOCK_STREAM, 0); //Create a TCP socket
	//if (socketFD < 0) fprintf(stderr, "CLIENT: ERROR CONNECTING\n");
	socketFD = createSocket();

	//Connect to server
	//if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
	//	fprintf(stderr, "CLIENT: ERROR CONNECTING\n");
	//};
	connectSocket(serverAddress, socketFD, sizeof(serverAddress));

	//Send greeting server
	sendMessage(greeting, socketFD, strlen(greeting));

	//Loop through receiving and sending message until \quit is received from server or if user inputs
	while (!isDone) {
		readMessage(buffer, socketFD, sizeof(buffer));
		if (checkForQuit(buffer))  {
			terminateConnection(&isDone);
		}
		else {
			printf("Server: %s\n", buffer);
			//Get next message from user
			memset(buffer, '\0', sizeof(buffer));
			printf("%s ", userHandle);
			fgets(buffer, sizeof(buffer), stdin);
			buffer[strcspn(buffer, "\n")] = '\0';
			sendMessage(buffer, socketFD, strlen(buffer));
			if (checkForQuit(buffer)) {
				terminateConnection(&isDone);
			};
		};
	};

	return 0;
}
