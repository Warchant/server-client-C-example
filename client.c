#include <stdlib.h>
#include <stdio.h>

#include <string.h>

#include <unistd.h> // for close

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void die(char* message){
	printf("%s\n", message);
	exit(1);
}


int main(int argc, char**argv){
	if (argc != 3) 
		die("Specify host and port: ./shell_cl <server-host> <server-port>");

	int port = 0;
	if(!(sscanf(argv[2], "%d", &port) == 1))
		die("Incorrect port. Specify a number. Example: 12345");
	

	int MasterSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == MasterSocket) 
		die("Can't open MasterSocket.");

	struct sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	if (!inet_aton(argv[1], &sa.sin_addr))
		die("Incorrect IP-address. Specify valid IP. Example: 192.168.1.2");	

	if( -1 == connect(MasterSocket, (struct sockaddr*)&sa, sizeof(sa))) 
		die("Can't connect to socket.");

	while(1){
		printf("$ ");

		char buffer[1000];
        fgets(buffer, sizeof(buffer), stdin);
        
        // send message to server
        if(send(MasterSocket, buffer, strlen(buffer), 0) < 0)
            die("Can't send message.");
         
        // receive message from server
        int len = recv(MasterSocket, buffer, sizeof(buffer), 0);
        if(len < 0)
            break;
    	else
    		buffer[len] = '\0';

        puts(buffer);
	}

	printf("Server closed the connection\n");

	shutdown(MasterSocket, SHUT_RDWR);
	close(MasterSocket);
	return 0;
}