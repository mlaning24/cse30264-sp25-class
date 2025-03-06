// Tracker.cc : Source code for the tracker

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <iostream>
using namespace std;

#include "Tracker.h"

Tracker::Tracker()
{
    // Default port is effectively an invalid one
    m_nPort = 0;
    // Nothing in the socket
    m_nSocket = -1;
}

Tracker::~Tracker()
{

}

bool Tracker::initialize ()
{
    int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	socklen_t addr_len;

    /* Double check that port indeed has been initialized */
    if (m_nPort == 0)
    {
        cerr << "Error: Unable to create a server on Port 0.  Make sure to properly initialize" << endl;
        cerr << "  the port setting for the server" << endl;
        return false;
    }

    /* Should check if we are root or just let it roll for a lower-level port? */


    // Set up the hints for UDP, any IP address type
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET; // set to AF_INET to use IPv4
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

    char szPortString[10];
    snprintf(szPortString, 10, "%d", getPort());

    // getaddrinfo gives us potential avenues for creating the socket that we will
    // use for the tracker

	if ((rv = getaddrinfo(NULL, szPortString, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return false;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("listener: socket");
			continue;
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("listener: bind");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "listener: failed to bind socket\n");
		return false;
	}

	freeaddrinfo(servinfo);

    m_nSocket = sockfd;

    return true;
}

void Tracker::go ()
{
    while(1)
    {
        Message * pRcvMessage;

        pRcvMessage = recvMessage();

        if (pRcvMessage != NULL) {
            switch(pRcvMessage->getType())
            {
                case MSG_TYPE_ECHO:
                    break;
                case MSG_TYPE_LIST_NODES:
                    break;
                case MSG_TYPE_REGISTER:
                    break;
                default:
                    // The client should not be sending these messages to us
                    break;
            }
        }
    }
}

Message * Tracker::recvMessage ()
{
    Message * pMessage;
	socklen_t addr_len;
	struct sockaddr_storage their_addr;
    int numbytes;

    pMessage = new Message ();

	printf("listener: waiting to recvfrom...\n");

	addr_len = sizeof(their_addr);

	if ((numbytes = recvfrom(getSocket(), pMessage->getData(), pMessage->getMaxLength() , 0,
		(struct sockaddr *)&their_addr, &addr_len)) == -1) {
		perror("recvfrom");
		exit(1);
	}

    /* Save the relevant info */
    pMessage->setLength((uint16_t) numbytes);

    cout << "Tracker: Received a packet of " << pMessage->getLength() << " long" << endl;

    if (isVerbose())
    {
        cout << "Binary Dump of Contents" << endl;
        // TODO: Add in a binary dump of the contents

        // TODO: Process the Type and Length contents
    }

    return pMessage;
}

bool Tracker::processEcho (Message * pMessageEcho)
{
    return false;
}

bool Tracker::processRegister (Message * pMessageRegister)
{
    return false;
}

bool Tracker::processListNodes (Message * pMessageListNodes)
{
    return false;
}
