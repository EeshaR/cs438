// /*
// ** client.c -- a stream socket client demo
// */

// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <errno.h>
// #include <string.h>
// #include <netdb.h>
// #include <sys/types.h>
// #include <netinet/in.h>
// #include <sys/socket.h>

// #include <arpa/inet.h>

// #define PORT "3490" // the port client will be connecting to 

// #define MAXDATASIZE 100 // max number of bytes we can get at once 

// // get sockaddr, IPv4 or IPv6:
// void *get_in_addr(struct sockaddr *sa)
// {
// 	if (sa->sa_family == AF_INET) {
// 		return &(((struct sockaddr_in*)sa)->sin_addr);
// 	}

// 	return &(((struct sockaddr_in6*)sa)->sin6_addr);
// }

// int main(int argc, char *argv[])
// {
// 	int sockfd, numbytes;  
// 	char buf[MAXDATASIZE];
// 	struct addrinfo hints, *servinfo, *p;
// 	int rv;
// 	char s[INET6_ADDRSTRLEN];

// 	if (argc != 2) {
// 	    fprintf(stderr,"usage: client hostname\n");
// 	    exit(1);
// 	}

// 	memset(&hints, 0, sizeof hints);
// 	hints.ai_family = AF_UNSPEC;
// 	hints.ai_socktype = SOCK_STREAM;

// 	if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
// 		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
// 		return 1;
// 	}

// 	// loop through all the results and connect to the first we can
// 	for(p = servinfo; p != NULL; p = p->ai_next) {
// 		if ((sockfd = socket(p->ai_family, p->ai_socktype,
// 				p->ai_protocol)) == -1) {
// 			perror("client: socket");
// 			continue;
// 		}

// 		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
// 			close(sockfd);
// 			perror("client: connect");
// 			continue;
// 		}

// 		break;
// 	}

// 	if (p == NULL) {
// 		fprintf(stderr, "client: failed to connect\n");
// 		return 2;
// 	}

// 	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
// 			s, sizeof s);
// 	printf("client: connecting to %s\n", s);

// 	freeaddrinfo(servinfo); // all done with this structure

// 	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
// 	    perror("recv");
// 	    exit(1);
// 	}

// 	buf[numbytes] = '\0';

// 	printf("client: received '%s'\n",buf);

// 	close(sockfd);

// 	return 0;
// }


/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string>
#include <iostream>
#include <fstream>
#include <iostream>

#include <arpa/inet.h>

#define PORT "3490" // the port client will be connecting to 

#define MAXDATASIZE 1024 // max number of bytes we can get at once 

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
	int sockfd, numbytes;  
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];
	std::string inp = argv[1];
	std::ofstream outputfile;

	if (argc != 2) {
	    fprintf(stderr,"usage: client hostname\n");
	    exit(1);
	}

	int startidx = 0;
    int endidx = inp.find("//", startidx)-1;

	// std::string pref = inp.substr(startidx, endidx);
	std::string remainder = inp.substr(endidx+3, inp.length());

	std::string dest = remainder;


	if(remainder.find("/", startidx)!=std::string::npos) {

		endidx = remainder.find("/", startidx)-1;

		dest = remainder.substr(startidx, endidx+1);
		remainder = remainder.substr(endidx+1, inp.length());
	} else {
		remainder = "/";
	}
    

	std::cout<<dest<<std::endl;

	// std::string portref(":");
	std::string portnum("80");

	if(dest.find(":") != std::string::npos) {
		endidx = dest.find(":", startidx);
		portnum = dest.substr(endidx+1, dest.length());
		dest = dest.substr(startidx, endidx);
		std::cout<<portnum<<std::endl;
		std::cout<<dest<<std::endl;
	}

	std::string get_req = "GET " + remainder + " HTTP/1.1\r\nUser-Agent: Wget/1.12 (linux-gnu)\r\nHost: " + dest + "\r\nConnection: Keep-Alive\r\n\r\n";

	std::cout<<get_req<<std::endl;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(dest.c_str(), portnum.c_str(), &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("client: connect");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure

  	outputfile.open("output", std::ios::binary);

	const char * inp_req = get_req.c_str();
	long req_length = get_req.size();
	int contiter = 0;
	int read_count;

	send(sockfd, inp_req, req_length, 0);
	memset(buf, '\0', MAXDATASIZE);
	int not_first_line = 0;

	while(!contiter) {
		if((read_count = recv(sockfd, buf, MAXDATASIZE, 0))<=0) {
			contiter = 1;
		} else {
			if(not_first_line) {
				long writesize = read_count*sizeof(char);
				outputfile.write(buf, writesize);
				std::cout<<buf<<std::endl;
			} else {
				not_first_line = 1;

				std::string str_buf(buf);
				endidx = str_buf.find("\r\n\r\n", startidx) + 4;
				str_buf = str_buf.substr(endidx, str_buf.length());
                outputfile.write(str_buf.c_str(), str_buf.length());
			}
		}

		memset(buf, '\0', MAXDATASIZE);
	}

	outputfile.close();

	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
	    perror("recv");
	    exit(1);
	}

	buf[numbytes] = '\0';

	printf("client: received '%s'\n",buf);

	close(sockfd);

	return 0;
}
