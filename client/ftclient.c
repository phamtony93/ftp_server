/***********************************************************************************************
 * Author: Tony
 * Program: Chat Client
 * Description: This program allows a user to connect to an ftp server and either request the
 * server's directory or request a specific file. This program takes 3 parameters for requesting
 * the directory and 4 for requesting a specific file.
 * Course: CS372 Intro to Networks
 * Last Modified: 3/8/2020
************************************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <dirent.h>

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
 * post: message from socket is read into the buffer. Amount of bytes read is returned
***********************************************************************************************/
int readMessage(char* buffer, int socketFD, int size) {
	int charsRead;
	memset(buffer, '\0', size);
	charsRead = recv(socketFD, buffer, size, 0);
	if (charsRead < 0) {
		fprintf(stderr, "CLIENT: ERROR reading from socket");
	};
	return charsRead;
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
	if (socketFD < 0) fprintf(stderr, "CLIENT: ERROR CREATING SOCKET\n");
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
		fprintf(stderr, "CLIENT: ERROR CONNECTING TO SOCKET\n");
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

/******************************************************************************************************
 * output 1 if number of commandline arguments does not equal 5 or 6 (including prgram name)
 * pre: program is executed and argc is passed to function
 * post: returns 1 if valid 0 otherwise
******************************************************************************************************/
int checkArgumentsCount(int count) {
	if (count == 5 || count ==6) {
		return 1;
	} else {
		printf("Invalid count of arguments\n");
		exit(1);
	};
};

/*****************************************************************************************************
 * checks current directory for a file with the same name
 * pre: pass the name of the file to be checked
 * post: returns 0 if no file found or 1 if file found
*****************************************************************************************************/
int checkForFile(char* fileName) {
	struct dirent *fileInDir;
	DIR* dirToCheck = opendir(".");

	if (dirToCheck > 0) {
		while ((fileInDir = readdir(dirToCheck)) != NULL) {
			if (strcmp(fileName, fileInDir->d_name) == 0) {
				return 1;
			};
		};

	return 0;
	};
};

/*****************************************************************************************************
 * Appends -copy to file name
 * pre: pass file name to have -copy appended
 * post: pointer to string 
*****************************************************************************************************/
void appendCopy(char* string, int length) {
	char* copy = "-copy.txt";
	strcat(string + length, copy); 
};

int main(int argc, char*argv[]) {
	int socketFD, portNumber, charsRead, socketFDData, portNumberData;
	struct sockaddr_in serverAddress;
	struct sockaddr_in serverAddressData;
	struct hostent* serverHostInfo;
	struct hostent* serverHostInfoData;
	char buffer[100000];
	char userHandle[1000];
	int isDone = 0;
	
	char temp[1000];

	//Validate parameters
	checkArgumentsCount(argc);

	//Initialize commandline parameters
	char fileName[10000];
	char command[200];
	char request[1000];

	memset(fileName, '\0', sizeof(fileName));
	memset(command, '\0', sizeof(command));
	memset(request, '\0', sizeof(request));

	strcpy(command, argv[3]);
	if (strcmp(command, "-g") == 0) {
		strcpy(fileName, argv[4]);
	};

	//prep initial message
	memset(request, '\0', sizeof(request));
	strcpy(request, argv[3]);
	request[strlen(request)] = ' ';
	strcat(request, argv[4]);
	if (argc > 5) {
		request[strlen(request)] = ' ';
		strcat(request, argv[5]);
	};

	//Get handle from user
	getHandleFromUser(userHandle);

	//set up the server address struct for control connection
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
	socketFD = createSocket();
	connectSocket(serverAddress, socketFD, sizeof(serverAddress));

	//Send command to server
	sendMessage(request, socketFD, strlen(request));
	readMessage(buffer, socketFD, sizeof(buffer));

	sleep(5);
	//set up the server address struct for data connection
	memset((char*)&serverAddressData, '\0', sizeof(serverAddressData)); //Clear out the address struct
	portNumberData = atoi(argv[argc - 1]); //Get the port number for data connection, convert to an integer from a string
	
	//Connect to server with new port
	serverAddressData.sin_family = AF_INET;
	serverAddressData.sin_port = htons(portNumberData); //Convert port number to network type
	serverHostInfoData = gethostbyname(argv[1]);
	memcpy((char*)&serverAddressData.sin_addr.s_addr, (char*)serverHostInfoData->h_addr, serverHostInfoData->h_length);


	socketFDData = createSocket();
	connectSocket(serverAddressData, socketFDData, sizeof(serverAddressData));

	int messageSize;
	int count = 0;
	readMessage(buffer, socketFDData, sizeof(buffer));

	close(socketFD);

	if (strcmp(command, "-l") == 0) {
		printf("Receiving directory structure from server on port %d\n", portNumberData);
		printf("%s", buffer);
	}
	else if (strcmp(command, "-g") == 0) {
		if (strcmp(buffer, "File Not Found") == 0) {
			printf("%s\n", buffer);
		}
		else {
			printf("Receiving %s from server\n", fileName);
			while (checkForFile(fileName)) {
				appendCopy(fileName, strlen(fileName));
			};
			FILE* outputFilePtr = fopen(fileName, "w");
			fprintf(outputFilePtr, buffer);
			fclose(outputFilePtr);
			printf("File transfer complete\n");
		}		
	};

	return 0;
}
