#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <arpa/inet.h>

#include <unistd.h> // for close

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <arpa/inet.h> // for inet_aton

void die(char* message){
	printf("%s\n", message);
	exit(1);
}

int main(int argc, char**argv){
	if (argc != 2) 
		die("Specify port: ./shell_sv <server-port>");

	int port = 0;
	if(!(sscanf(argv[1], "%d", &port) == 1))
		die("Incorrect port. Specify a number. Example: 12345");

	int MasterSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == MasterSocket) 
		die("Can't open MasterSocket.");
	
	struct sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);              // any valid port
	sa.sin_addr.s_addr = htonl(INADDR_ANY); // 0.0.0.0

	if(-1 == bind(MasterSocket, (struct sockaddr*)&sa, sizeof(sa))) 
		die("Can't bind MasterSocket to given port");

	if(-1 == listen(MasterSocket, SOMAXCONN)) 
		die("Can't start listening");

	printf("### Server has started ###\n");

	while(1) {
		struct sockaddr_in client; 
		socklen_t client_len;
		int SlaveSocket = accept(MasterSocket, (struct sockaddr*)&client, &client_len);
		
		// work with the separate clients in the separate processes
		int client_pid = -1;
		if(-1 == (client_pid = fork())){
			printf("Can't create process. Something terrible hapenned. Waiting 10 seconds...\n");
			sleep(10);
		}

		if (client_pid == 0) { // child. 
			printf("Client %s connected\n", inet_ntoa(client.sin_addr));

			// redirect stdout and stderr to the client's socket
			dup2(SlaveSocket, 0);
			dup2(SlaveSocket, 1);
			dup2(SlaveSocket, 2);

			execl("/bin/sh", "shell_sv", NULL);

			printf("Client %s disconnected\n", inet_ntoa(client.sin_addr));
		}
		else{
			close(SlaveSocket);
		}
	}	

	shutdown(MasterSocket, SHUT_RDWR);
	close(MasterSocket);
	return 0;
}