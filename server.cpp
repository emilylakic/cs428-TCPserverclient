#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>

#define PORT 4444

int main() {
	char backupBuffer[1024];
	int messagesReceived = 0;
	struct timeval first;
	int firstMilleseconds;
	int firstFd;
	int opt = 1;
	int sockfd;
	int max_sd;
	struct sockaddr_in serverAddr;
	int newSocket;
	int client_socket[30];
	int max_clients = 30;
	int activity, valread, sd;
	struct sockaddr_in newAddr;
	socklen_t addr_size;
	char buffer[1024];
	char currentTime2[84];
	char str[100] = "X: Emily received before Y: Tyler";
	pid_t childpid;
	//Set of socket descriptors
	fd_set readfds;
	int addrlen;

	//Initialize all client_socket[] to 0, so not checked
  for (int i = 0; i < max_clients; i++){
  	client_socket[i] = 0;
  }

	//Create socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		printf("Socket Failed.\n");
		exit(1);
	}
	//Initializing server information
	//Assign IP and PORT
	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	//Binding newly created socket
	if(bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
		printf("Error in binding.\n");
		exit(1);
	}
	int l = strlen(str);
	if (listen(sockfd, 10) < 0) {
        printf("Listen");
        return(1);
  }
	addrlen = sizeof(serverAddr);
	while(1) {
		//Clear the socket set
		FD_ZERO(&readfds);
		//Add socket (sockfd) to set
		FD_SET(sockfd, &readfds);
		max_sd = sockfd;
		//Add child sockets to set
		for (int i = 0; i < max_clients; i++) {
			//Socket descriptor
			sd = client_socket[i];
			//If the socket descriptor is valid, then add it to read list
			if(sd > 0) {
				FD_SET( sd, &readfds);
			}
			//Highest file descriptor (FD) number. Needed for the select function
			if(sd > max_sd) {
				max_sd = sd;
			}
		}
		//Wait for an activity on one of the sockets
		//Timeout is NULL, so wait indefinitely
    activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
		if ((activity < 0) && (errno!=EINTR)) {
            printf("select error");
    }
		//If something happened on the sockfd socket, then it is an incoming connection
		if (FD_ISSET(sockfd, &readfds)) {
	    if ((newSocket = accept(sockfd,
	            (struct sockaddr *)&serverAddr, (socklen_t*)&addrlen))<0) {
	        perror("accept");
	        exit(EXIT_FAILURE);
	    }
			//Inform user that the connection was accepted
			//Inform user of the socket fd, the ip address, and the port number
    	printf("Accepted Connection. Socket FD: %d, IP Address: %s, Port: %d\n" , newSocket , inet_ntoa(serverAddr.sin_addr) , ntohs(serverAddr.sin_port));
			//Add new socket to array of sockets
      for (int i = 0; i < max_clients; i++) {
          //If position is empty
          if(client_socket[i] == 0) {
	          client_socket[i] = newSocket;
						//Gets the time of the message received from the client
						struct timeval now;
						gettimeofday(&now, NULL);
						//In milliseconds for precision
						int milli2 = now.tv_usec / 1000;
						//newerBuffer[] to allow for formatting in Y-%m-%d %H:%M:%S
						char newerBuffer[80];
						strftime(newerBuffer, 80, "%Y-%m-%d %H:%M:%S", localtime(&now.tv_sec));//
						//currentTime[] to store Y-%m-%d %H:%M:%S (newerBuffer[]), now with milliseconds included (milli2)
						char currentTime2[84] = "";
						//Receive data from client, buffer from both Client X and Client Y
						recv(newSocket, buffer, 1024, 0);
						sprintf(currentTime2, "%s:%03d", newerBuffer, milli2);
						//Will print 'Client X: Emily' and 'Client Y: Tyler', respectively
						printf("Client %s\n", buffer);

					if(messagesReceived > 0) {
						//Comparison between the time received for each client's message completed here
						char blank[100];
						//Used timercmp to compare the two times
						int comparisonValue = timercmp(&now,&first,>);
						if (comparisonValue > 0) {
							sprintf(blank,"%s recieved before %s",backupBuffer,buffer);
						} else {
							sprintf(blank,"%s received before %s",buffer,backupBuffer);
						}
						strncpy(str,blank,100);
						if(send(newSocket, str, strlen(str), 0) != strlen(str)) {
	              printf("Send\n");
	          }
						//String sent to Client X and Client Y on whether X or Y was received before
						send(firstFd, str, strlen(str), 0);
						//Printed ACK on server side after the above string was sent by the server and received by the client
						printf("Sent acknowledgment to both X and Y\n");
						return 0;
						}
						else {
							strncpy(backupBuffer,buffer,1024);
							messagesReceived++;
							first = now;
							firstMilleseconds = milli2;
							firstFd = newSocket;
							}
            break;
            }
        	}
      	}
				//Otherwise it is some IO operation on some other socket
        for (int i = 0; i < max_clients; i++) {
	        sd = client_socket[i];
	        if(FD_ISSET(sd, &readfds)) {
	          //Check if it was for closing
						//Read the incoming message
	          if((valread = read(sd, buffer, 1024)) == 0) {
	              getpeername(sd, (struct sockaddr*)&serverAddr, \
	                  (socklen_t*)&addrlen);
	              printf("Host Disconnected. IP Address: %s, Port: %d\n",
	                    inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port));
	              //Close the socket and mark as 0 in list for reuse
	              close(sd);
	              client_socket[i] = 0;
	          }
            //Echo back the message that came in
            else {
            //Set the string terminating NULL byte on the end of the data read
            buffer[valread] = '\0';
            send(sd, buffer, strlen(buffer), 0 );
						}
        	}
        }
    }
  return 0;
}
//Would like to acknowledge the site https://www.geeksforgeeks.org/socket-programming-in-cc-handling-multiple-clients-on-server-without-multi-threading/ for help on how to handle two clients on my server without multithreading
