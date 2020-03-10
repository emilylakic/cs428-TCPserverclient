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

int main(){

	int clientSocket, ret;
	struct sockaddr_in serverAddr;
	char buffer[1024] = "Y:Tyler";
	char currentTime2[84] = "";
	char str[1024] = "Client Y:Tyler";

	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(clientSocket < 0){
		printf("[-]Error in connection.\n");
		exit(1);
	}
	//printf("[+]Client Socket is created.\n");

	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	ret = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(ret < 0){
		printf("[-]Error in connection.\n");
		exit(1);
	}
	//printf("[+]Connected to Server.\n");

	while(1) {
		if(send(clientSocket, buffer, strlen(buffer), 0)) {
			//printf("Server: %s\n", buffer);
			printf("Sent = %s\n", str);
			//printf(currentTime2, "%s:%03d\n", newerBuffer, milli2);
		}
		recv(clientSocket, currentTime2, 84, 0);
		printf("%s\n", currentTime2);

		if(strcmp(buffer, ":exit") == 0) {
			close(clientSocket);
			printf("[-]Disconnected from server.\n");
			exit(1);
		}

		if(recv(clientSocket, buffer, 1024, 0) < 0) {
			printf("[-]Error in receiving data.\n");
		}else{
			//printf("Server: \t%s\n", buffer);
		}
	}

	return 0;
}
