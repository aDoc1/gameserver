#include <stdio.h>
#include <strings.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <signal.h>
#include "MyServer.h"

//#define ERROR      42
//#define LOG        44
//#define FORBIDDEN 403
//#define NOTFOUND  404
#define BUFFSIZE 2048

int sockfd; // The socket pointer you'll use
void cleanExit() {
	// close sockfd
	close(sockfd);
	exit(0);
}

/*void logger(int type, char *s1, char *s2, int socket_fd)
{
	int fd ;
	char logbuffer[BUFFSIZE*2];

	switch (type) {
	case ERROR: 
		sprintf(logbuffer,"ERROR: %s:%s Errno=%d exiting pid=%d",s1, s2, errno,getpid()); 
		break;
	case FORBIDDEN: 
		write(socket_fd, "HTTP/1.1 403 Forbidden\nContent-Length: 185\nConnection: close\nContent-Type: text/html\n\n<html><head>\n<title>403 Forbidden</title>\n</head><body>\n<h1>Forbidden</h1>\nThe requested URL, file type or operation is not allowed on this simple static file webserver.\n</body></html>\n",271);
		sprintf(logbuffer,"FORBIDDEN: %s:%s",s1, s2); 
		break;
	case NOTFOUND: 
		write(socket_fd, "HTTP/1.1 404 Not Found\nContent-Length: 136\nConnection: close\nContent-Type: text/html\n\n<html><head>\n<title>404 Not Found</title>\n</head><body>\n<h1>Not Found</h1>\nThe requested URL was not found on this server.\n</body></html>\n",224);
		sprintf(logbuffer,"NOT FOUND: %s:%s",s1, s2); 
		break;
	case LOG: (void)sprintf(logbuffer," INFO: %s:%s:%d",s1, s2,socket_fd); 
		break;
	}	
	/* No checks here, nothing can be done with a failure anyway 
	if((fd = open("adweb.log", O_CREAT| O_WRONLY | O_APPEND,0644)) >= 0) {
		(void)write(fd,logbuffer,strlen(logbuffer)); 
		(void)write(fd,"\n",1);      
		(void)close(fd);
	}
	if(type == ERROR || type == NOTFOUND || type == FORBIDDEN) 
		exit(3);
}*/


int main(int argc, char *argv[]) {
	signal(SIGTERM, cleanExit);
	signal(SIGINT, cleanExit);
	int newsockfd; // The socket you get with a connection
	int port; // The port you listen on
	int clilen; // The length of the client's address
	char* res; // stores the result
	int yes = 1;
	int pid;

	struct sockaddr_in serv_addr; // The server's address
	struct sockaddr_in cli_addr; // The client's address

	char buffer[BUFFSIZE]; // a buffer to read results to.
	int n; // number of bytes read
	
	/* Become deamon + unstopable and no zombies children (= no wait()) */
	if(fork() != 0)
	{
		return 0; /* parent returns OK to shell */
	}
	signal(SIGCLD, SIG_IGN); /* ignore child death */
	signal(SIGHUP, SIG_IGN); /* ignore terminal hangups */	
	setpgrp();		/* break away from process group */
	// set clilent to size of client address struct
	clilen = sizeof(cli_addr);

	// allocate sockfd by calling 'socket'
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Could not open socket");
		return EXIT_FAILURE;
	}

	// You should look up what this does if you don't know.
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void*)&yes, sizeof(int) );

	// Set the port number based on the first command line argument
	port = (argc > 1) ? atoi(argv[1]) : 8000;

	// Zero-out the server address
	bzero((void*) &serv_addr, sizeof(serv_addr));

	// initialize the socket structure (i.e., set the values of serv_addr)
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);

	// bind the host address
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("ERROR on binding");
		return EXIT_FAILURE;
	}
	
	printf("IP of server: %s\n",(char *)serv_addr.sin_addr.s_addr);
	
	// Listen for a client.
	listen(sockfd, 64);

	while (1) { // go forever!
		// Accept actual connection from the client
		if ((newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen)) < 0) {
			perror("ERROR on accept");
			return EXIT_FAILURE;
		}
 
		if((pid = fork()) < 0) {
			perror("error on fork");
			return EXIT_FAILURE;
		}
		else {
			if(pid == 0) { 	/* child */
				close(sockfd);
				if ((n = read(newsockfd, buffer, BUFFSIZE, 0)) > 0) {
					printf("%s", buffer);
					res = handle_request(buffer);
					write(newsockfd, res, strlen(res),0);
					bzero(buffer, BUFFSIZE);
					free(res);
				}
			} else { 	/* parent */
				close(newsockfd);
			}
		}
		// close newsockfd
		close(newsockfd);
	}
	// close sockfd
	close(sockfd);
	exit(0);
}

