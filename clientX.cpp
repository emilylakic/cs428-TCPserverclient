#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>

#define PORT 4444

int main() {
	int clientSocket;
	struct sockaddr_in serverAddr;
	char buffer[1024] = "X: Emily";
	char currentTime2[84] = "";
	char str[1024] = "Client X: Emily";
	//Create socket
	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
		if(clientSocket < 0) {
			printf("Connection Error.\n");
			exit(1);
		}
	//Initializing information
	//Assign IP and PORT
	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	//Connection of client to server
		if(connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
			printf("Connection Error.\n");
			exit(1);
			}
	//If the message with the client's name (stored in buffer[]) was sent to the server, then print out on the client's terminal that the message was sent with the respective message sent, stored in str[]
	while(1) {
		if(send(clientSocket, buffer, strlen(buffer), 0)) {
			printf("Message Sent = %s\n", str);
		}
		//Prints out message received from server
		if(recv(clientSocket, currentTime2, 84, 0)) {
			printf("%s\n", currentTime2);
			return 0;
		}
		if(strcmp(buffer, ":exit") == 0) {
			close(clientSocket);
			printf("Disconnected from Server.\n");
			exit(1);
		}
		if(recv(clientSocket, buffer, 1024, 0) < 0) {
		} else {
		}
	}
	return 0;
}
