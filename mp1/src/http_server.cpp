/*
** server.c -- a stream socket server demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#define PORT "3490"  // the port users will be connecting to

#define BACKLOG 10	 // how many pending connections queue will hold

void sigchld_handler(int s)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

// get sockaddr, IPv4 or IPv6: 
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void checkRequest(int client_fd, const std::string& request) {
	
	std::string header; //saves the HTTP header 

    std::istringstream curr_stream(request); //getting the stream 
    std::string code, file_path, http_version;
	curr_stream >> code; //storing the type of request
	curr_stream >> file_path; //storing the location of the file path (from current directory)
	curr_stream >> http_version; //storing the version 

    if (code == "GET") 
	{
		//creating an input file stream to read the given file 
		int mode_type = std::ios::in | std::ios::binary; //open for reading in binary (not text) mode
		std::ifstream get_file("." + file_path, mode_type); 

		//deciding the appropriate header 
        if (get_file) 
		{
            std::ostringstream content;
            content << get_file.rdbuf(); //reading in the content of the file
            header = "HTTP/1.1 200 OK\n" + content.str() + "\n"; //add new line 
        } 
		else 
		{
            header = "HTTP/1.1 404 Not Found\n";
			//question - does this also have content?
        }
    } 
	else //for example, if "POST" 
	{
        header = "HTTP/1.1 400 Bad Request\n";
    }

    //send the http response back to the client 
    if (send(client_fd, header.c_str(), header.size(), 0) == -1) {
        perror("send");
    }
}

//The original code uses a fixed port number, but the server should be able to take in any port number
int main(int argc, char *argv[])
{
	//check if the port number is given 
	if (argc != 2) {
        fprintf(stderr,"port error\n"); //change this later
        exit(1);
    }
 
	int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	int yes=1;
	char s[INET6_ADDRSTRLEN];
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	//don't use PORT, use argv[1] to send in port address 
	if ((rv = getaddrinfo(NULL, argv[1], &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	if (p == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		return 2;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	printf("server: waiting for connections...\n");

	while(1) {  // main accept() loop
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			continue;
		}

		inet_ntop(their_addr.ss_family,
			get_in_addr((struct sockaddr *)&their_addr),
			s, sizeof s);
		printf("server: got connection from %s\n", s);

		if (!fork()) { // this is the child process
			close(sockfd); // child doesn't need the listener

			// //http request logic 
			// if (send(new_fd, "Hello, world!", 13, 0) == -1)
			// 	perror("send");

			char buffer[512]; //TA mentioned passing in 512 byte chunks
            ssize_t bytes_received = recv(new_fd, buffer, sizeof(buffer) - 1, 0); //new_fd is file descriptor, buffer is where data will be stored, 
            if (bytes_received <= 0) 
			{
                perror("recv");
            } 
			else 
			{
                buffer[bytes_received] = '\0'; // null-terminate the string
                std::string request(buffer); //make the buffer a string (memory issues?) 
                checkRequest(new_fd, request);
            }
			
			close(new_fd);
			exit(0);
		}
		close(new_fd);  // parent doesn't need this
	}

	return 0;
}

/*
Steps: 
1. Take the port as a command-line argument.
2. Return the HTTP response codes along with document text.
*/

/*
Resources: 
Stack Overflow Articles: 
- "Socket programming in C++"
- "How to handle HTTP request in C++"
- "C++ std::string vs C-style string"
- "Handling multiple client connections in socket programming"
*/
