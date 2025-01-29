
#include <pthread.h>
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
using namespace std;

#include "Server.h"
#include "Client.h"
#include "Thread_Output.h"

#include "SocketHelper.h"

/* Global Defines */
#define BUFFER_MAX 1000

// get sockaddr, IPv4 or IPv6:
void * get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void * Thread_Lobby (void * pData)
{
	Server * pServer;
	int 	 nLobbySocket;

	pServer = (Server *) pData;

	nLobbySocket = createSocket_TCP_Listen(pServer->getPort_Lobby());
	pServer->setSocket_Lobby(nLobbySocket);

	if(!nLobbySocket)
	{
		cerr << "Error: Unable to establish the lobby socket into a listen state" << endl;
		exit(-1);
	}

	// Adapting this from Beej's Guide
	
    //int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;

	queueOutput("Server (Lobby): Waiting for connections .... \n");

    while(pServer->keepLobbyRunning()) 
	{  
		int		nClientSocket;
		char 	szOutputBuf[1000];

        sin_size = sizeof their_addr;		
        nClientSocket = accept(pServer->getSocket_Lobby(), (struct sockaddr *)&their_addr, &sin_size);
		
        if (nClientSocket == -1) 
		{
            perror("accept");
            continue;
        }

		/* Simple bit of code but this can be helpful to detect successful
		   connections 
		 */
        inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof s);

		queueOutput("Server (Lobby): Got Connection from (");
		queueOutput(s);
		queueOutput("\n");


		// Print out this client information 
//		sprintf(theClientInfo.szIdentifier, "%s-%d", s, nClientCount);
//		theClientInfo.socketClient = new_fd;
//		nClientCount++;

		/* Make a client instance to handle this client */
		GameClient *	pClient;

		pClient = new GameClient ();

		// Get the socket set up for this client
		pClient->setSocket_Lobby(nClientSocket);

		// Set the server in class the client needs help
		pClient->setServer(pServer);

		// Start a thread to handle this client
		pClient->spawnThread_ClientLobby();
    }

	// Clean up the socket
	close(pServer->getSocket_Lobby());

	return NULL;
}



