#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <netinet/in.h>
#include <errno.h>

#define SUBNET "192.168.1.%d"
#define MAX_HOSTS 254

int main()
{
	
	char buffer[666];
	int i, count, ret;
	struct sockaddr_in servaddr;
	int socks[MAX_HOSTS], maxfd=0;
	char buf[30+1];
	fd_set fds;
	struct timeval timeout;

	/* Set up a timeout of 5 second */
	timeout.tv_sec = 5;
	timeout.tv_usec = 0;

	/* Partially set up the address structure */
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(1030);

	FD_ZERO ( &fds );
	
	/* Attempt to connect to all nodes on the subnet */
	for (i = 1; i <= MAX_HOSTS; i++)
	{
		/* Complete the address structures */
		sprintf(buf, SUBNET, i);
		servaddr.sin_addr.s_addr = inet_addr(buf);

		/* Create a new socket for this node */
		socks[i] = socket(AF_INET, SOCK_STREAM, 0);

		/* Add it to the socket descriptor list */
		FD_SET(socks[i], &fds);

		/* Make the socket nonblocking */
		fcntl(socks[i], F_SETFL, O_NONBLOCK);

		/* Attempt a connect (nonblocking) */
		connect(socks[i], (struct sockaddr_in *)&servaddr, sizeof(servaddr));
		
		if(socks[i] > maxfd) maxfd = socks[i];
	}

	count = MAX_HOSTS;
	
	while(1)
	{
		/* Await connect reponses (writable sockets) */
		ret = select (maxfd, NULL, &fds, NULL, &timeout);

		if(ret > 0)
		{
			/* Walk through the sector descriptor list */
			for(i = 1; i <= MAX_HOSTS; i++)
			{
				if(FD_ISSET(socks[i], &fds))
				{
					printf("Port open at 192.168.1.%d\n", i);
					FD_CLR(socks[i], &fds);
				}
			}

			/* If We've found everything, exit */
			if(count == 0)
				break;
		}
		else
		{
			/* Timeout, exit */
			break;
		}
	}

	/* Cleanup, close all of the sockets */
	for(i = 1; i < MAX_HOSTS; i++)
	{
		close(socks[i]);
	}

	int status;
	int buflen=sizeof(buffer);
	status=send(socks, buffer, buflen, MSG_DONTWAIT);
	if(status==-1)
	{
		/*send failed*/
		printf("send failed: %s\n", strerror(errno));
	}	
	else
	{
		/*send succeeded -- or did it?*/
		printf("Success");
	}

	status=read(socks, buffer, buflen);
	
	if(status > 0)
	{
		/*Error, check errno, take action*/
		printf("ERRNO");
	}
	else if(status==-1)
	{
		/*Peer closed the socket, finish the close*/
		close(socks);
	}
	
	int on;
	
	on=1;
	ret=setsockopt(socks, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	/*Allow connection to port 8080 from any available interface*/
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servaddr.sin_port=htons(45000);

	/*Bind to the address (interface/port) */
	ret=bind(socks, (struct sockaddr *)&servaddr, sizeof(servaddr));

	struct sockaddr_in saddr;
	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family=AF_INET;
	saddr.sin_port=htons(80);
	saddr.sin_addr.s_addr=inet_addr("192.168.1.1");

	return(0);
}
