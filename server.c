#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	int sockt;
	struct sockaddr_in server;
	int mysock1;
	char buff[1028];
	int rvall;

	sockt = socket(AF_INET, SOCK_STREAM, 0);
	if(sockt < 0)
	{
		perror("FAILED to CREATE SOCKET");
		exit(1);
	}

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port =htons(1028);

	
	if(bind(sockt , (struct sockaddr *)&server, sizeof(server)))
	{
		perror("BIND FAILED");
		exit(1);
	}

	
	listen(sockt, 5);

	do{
		mysock1 = accept(sockt, (struct sockaddr *) 0, 0);
		if(mysock1 == -1) 
		{
			perror("ACCEPT FAILED");
		}
		else
		{
			memset(buff, 0, sizeof(buff));
			if((rvall = recv(mysock1, buff, sizeof(buff), 0)) < 0)
				perror("reading stream message error");
			else if (rvall == 0)
				printf("ENDING CONNECTION\n");
			else
				printf("MSG: %s\n", buff);

			printf("Got The Message (rvall = %d)\n", rvall);
			close(mysock1);
		}
	}while (1);

	return 0;

}
