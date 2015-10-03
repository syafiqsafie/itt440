#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define DATA "SOCKET PROGRAMMING"

int main(int argc, char *argv[])
{
	
	int sockt;
	struct sockaddr_in server;
	struct hostent *hp, *gethostbyname();
	char buff[1028];

	sockt = socket(AF_INET, SOCK_STREAM, 0);
	if(sockt < 0)
	{
		perror("SOCKET FAILED");
		exit(1);
	}
	
	server.sin_family = AF_INET;

	hp = gethostbyname(argv[1]);
	if(hp == 0)
	{
		perror("gethostbyname FAILED");
		close(sockt);
		exit(1);
	}

	memcpy(&server.sin_addr, hp->h_addr, hp->h_length);
	server.sin_port = htons(1028);

	if(connect(sockt, (struct sockaddr *) &server, sizeof(server)) < 0)
	{
		perror("CONNECTION FAILED");
		close(sockt);
		exit(1);
	}

	if(send(sockt, DATA, sizeof(DATA), 0) <0)
	{
		perror("SEND FAILED");
		close(sockt);
		exit(1);
	}

	printf("Sent %s\n", DATA);
	close(sockt);

	return 0;
}
