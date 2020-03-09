#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>


#define PORT 4444

int main() {
	int messagesReceived = 0;
	int sockfd, ret;
	struct sockaddr_in serverAddr;
	int newSocket;
	struct sockaddr_in newAddr;
	socklen_t addr_size;
	char buffer[1024];
	char currentTime2[84];
	char str[100] = "Tester";
	pid_t childpid;
	//creating socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Server Socket is created.\n");
	//initializing server information
	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	//bind the socket to the port maybe?
	ret = bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(ret < 0){
		printf("[-]Error in binding.\n");
		exit(1);
	}
	printf("[+]Bind to port %d\n", 4444);
	int l = strlen(str);
	if(listen(sockfd, 10) == 0){
		printf("[+]Listening....\n");
	} else {
		printf("[-]Error in binding.\n");
	}


	//STARTING TO LISTEN--------------------
	while(1) {
		//creating a new socket
		newSocket = accept(sockfd, (struct sockaddr*)&newAddr, &addr_size);
		if(newSocket < 0){
			exit(1);//if the socket was not opened sucessfuly, exit
		}
		printf("Connection accepted from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));//print out where connection is from

		// possibly forking?
		if((childpid = fork()) == 0) {
			close(sockfd);

			while(1) {
				//creating timeval structure
				struct timeval now;
				gettimeofday(&now, NULL);//seting time value to now
				int milli2 = now.tv_usec / 1000;//setting milleseconds of time value
				char newerBuffer[80];
				strftime(newerBuffer, 80, "%Y-%m-%d %H:%M:%S", localtime(&now.tv_sec));//
				char currentTime2[84] = "";
				recv(newSocket, buffer, 1024, 0);
				sprintf(currentTime2, "%s:%03d", newerBuffer, milli2);
				messagesReceived++;
				printf("Messages Received: %d\n",messagesReceived);
				//recv(newSocket, currentTime2, 1024, 0);
				if(strcmp(buffer, ":exit") == 0) {
					printf("Disconnected from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
					//possibly return 1; here
					break;
				} else {
					send(newSocket, currentTime2, strlen(currentTime2), 0);
					printf("Client: %s\n", buffer); //this is where Client: is posted at server
					//printf("Time Received: %s\n", currentTime2); //this is where Client: is posted at server
					//printf("Message was received at: %s\n", currentTime2);
					//printf("Who printed first? %s\n", currentTime2);
					send(newSocket, buffer, strlen(buffer), 0);
					//send(newSocket, str, strlen(str), 0);
					ret = read(newSocket, str, l); //this must b after send
					bzero(buffer, sizeof(buffer));
					//bzero(currentTime2, sizeof(currentTime2));
				}
				// close(newSocket);
				// messagesReceived++;
				// printf("Messages Received: %d\n",messagesReceived);
				// //close(newSocket);
				// while(1) {
				// 	//printf("Message was received at: %s\n", currentTime2);
				//
				// }
			}
		}
	}
	close(newSocket);
	return 0;
}
