#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   //close
#include <arpa/inet.h>    //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros

#define PORT 4444

int main() {
	// stuff
	char backupBuffer[1024];
	int messagesReceived = 0;
	struct timeval first;
	int firstMilleseconds;
	int firstFd;
	int opt = 1;
	int sockfd, ret; //master_socket
	int max_sd; //max_sd
	struct sockaddr_in serverAddr; //address
	int newSocket; //new_socket
	int client_socket[30];//MAYBE ACTUALLY INITIALIZE THIS
	int max_clients = 30;
	int activity, valread, sd;
	struct sockaddr_in newAddr;
	socklen_t addr_size;
	char buffer[1024];
	char currentTime2[84];
	char str[100] = "X: Emily received before Y: Tyler";
	pid_t childpid;
	fd_set readfds;//look into structure
	int addrlen; //addrlen

	//initialise all client_socket[] to 0 so not checked
  for (int i = 0; i < max_clients; i++){
  	client_socket[i] = 0;
  }

	//creating socket
	//sockfd is the 'master socket'
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		printf("Socket Failed.\n");
		exit(1);
	}
	//printf("Server Socket is created: %d\n",sockfd);
	//initializing server information
	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	//bind the socket to the port maybe?
	ret = bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(ret < 0) {
		printf("Error in binding.\n");
		exit(1);
	}
	int l = strlen(str);
	if (listen(sockfd, 10) < 0){
        printf("listen");
        return(1);
  }

	addrlen = sizeof(serverAddr);

	//STARTING TO LISTEN
	while(1) {
		//clear the socket set
		FD_ZERO(&readfds);
		FD_SET(sockfd, &readfds);
		max_sd = sockfd;
		for (int i = 0 ; i < max_clients ; i++){
		//socket descriptor
		sd = client_socket[i];
		//if valid socket descriptor then add to read list
		if(sd > 0){
			FD_SET( sd , &readfds);
		}

		//highest file descriptor number, need it for the select function
		if(sd > max_sd){max_sd = sd;}
		}
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);

        if ((activity < 0) && (errno!=EINTR))
        {
            printf("select error");
        }

				        if (FD_ISSET(sockfd, &readfds))
				        {
				            if ((newSocket = accept(sockfd,
				                    (struct sockaddr *)&serverAddr, (socklen_t*)&addrlen))<0)
				            {
				                perror("accept");
				                exit(EXIT_FAILURE);
				            }
            printf("Accepted Connection. Socket FD: %d, IP Address: %s, Port: %d\n" , newSocket , inet_ntoa(serverAddr.sin_addr) , ntohs(serverAddr.sin_port));

            for (int i = 0; i < max_clients; i++) {
                //if position is empty
                if( client_socket[i] == 0 ) {
                    client_socket[i] = newSocket;
										struct timeval now;
										gettimeofday(&now, NULL);//seting time value to now
										int milli2 = now.tv_usec / 1000;//setting milleseconds of time value
										char newerBuffer[80];
										strftime(newerBuffer, 80, "%Y-%m-%d %H:%M:%S", localtime(&now.tv_sec));//
										char currentTime2[84] = "";
										recv(newSocket, buffer, 1024, 0);
										sprintf(currentTime2, "%s:%03d", newerBuffer, milli2);
										printf("Client %s\n", buffer);
										//printf("SD: %d\n",sd);
										if(messagesReceived>0){
											//comparison done here
											char blank[100];
											int comparisonValue = timercmp(&now,&first,>);
											if (comparisonValue>0){
												sprintf(blank,"%s recieved before %s",backupBuffer,buffer);
											}else{
												sprintf(blank,"%s received before %s",buffer,backupBuffer);
											}
											//printf("Value of blank: %s\n",blank);
											strncpy(str,blank,100);
											//printf("Value of str after copy(ready to send): %s\n",str);


											//--------Playing with this shit to next line-------------------


											if( send(newSocket, str, strlen(str), 0) != strlen(str) ){
					                printf("Send\n");
					            }
											//printf("SENT THE FIRST MESSAGE\n");
											//printf("SOCKFD: %d\n",sockfd);

											// if ((newSocket = accept(sockfd,(struct sockaddr *)&serverAddr, (socklen_t*)&addrlen))<0){
					            //     perror("accept");
					            //     exit(EXIT_FAILURE);
					            // }else{
											// 	printf("Socket Created!\n");
											// }

	            				//printf("Accept connection, socket fd is %d, ip is: %s, port: %d\n" , newSocket , inet_ntoa(serverAddr.sin_addr) , ntohs(serverAddr.sin_port));

											send(firstFd, str, strlen(str), 0);
									    //--------------------------------------------------------------
											printf("Sent acknowledgment to both X and Y\n");
											//close(firstFd);
											//close(sd);
											//close(sockfd);
											//close(newSocket);
											return 0;

										}else{
											strncpy(backupBuffer,buffer,1024);
											messagesReceived++;
											first=now;
											firstMilleseconds=milli2;
											firstFd=newSocket;
											//printf("firstFd: %d\n",firstFd);
										}
                    break;
                }
            }
        }

				//else its some IO operation on some other socket
        for (int i = 0; i < max_clients; i++){
            sd = client_socket[i];
            if (FD_ISSET( sd , &readfds)){
                //Check if it was for closing , and also read the
                //incoming message
                if ((valread = read( sd , buffer, 1024)) == 0)
                {
                    getpeername(sd , (struct sockaddr*)&serverAddr , \
                        (socklen_t*)&addrlen);
                    printf("Host disconnected , ip %s , port %d \n" ,
                          inet_ntoa(serverAddr.sin_addr) , ntohs(serverAddr.sin_port));

                    //Close the socket and mark as 0 in list for reuse
                    close( sd );
                    client_socket[i] = 0;
                }

                //Echo back the message that came in
                else
                {
                    //set the string terminating NULL byte on the end
                    //of the data read
                    buffer[valread] = '\0';
                    send(sd , buffer , strlen(buffer) , 0 );//maybe comment?
                }
            }
        }
    }

  return 0;
}
