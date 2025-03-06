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
#include <sys/time.h>

#include <iostream>
using namespace std;

#include "Tracker.h"
#include "utils.h"

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

bool Tracker::initialize (char * pszIP)
{
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

    if ((rv = getaddrinfo(pszIP, szPortString, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return false;
    }

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((m_nSocket = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("initialize: socket");
			continue;
		}

        if (bind(m_nSocket, p->ai_addr, p->ai_addrlen) == -1) {
            close(m_nSocket);
            perror("initialize: bind");
            continue;
        }

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "initialize: failed to properly set up socket\n");
		return false;
	}

    // Copy in the respective information for the socket
    memcpy(getAddressInfo(), p->ai_addr, sizeof(struct sockaddr_in));

    // A few notes on addrinfo and sockaddr
    //
    // addrinfo contains lots of information, part of which is a pointer to
    // sockaddr content
    //
    // sockaddr is a "container" that has 16 bytes in total.  It is the same size
    // as a sockaddr_in struct which has enough to hold an IPv4 address as well as
    // the accompanying port and other information

    /*
    struct sockaddr_in {
        short int          sin_family;  // Address family, AF_INET
        unsigned short int sin_port;    // Port number
        struct in_addr     sin_addr;    // Internet address
        unsigned char      sin_zero[8]; // Same size as struct sockaddr
    };
    */

    if(isVerbose())
    {
        dump_sockaddr_in(getAddressInfo());
    }

	freeaddrinfo(servinfo);
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
                    processEcho(pRcvMessage);
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
	struct sockaddr clientAddr;
    int numbytes;

    /* Create a new message object */
    pMessage = new Message ();

	printf("listener: waiting to recvfrom...\n");

	addr_len = sizeof(clientAddr);

	if ((numbytes = recvfrom(getSocket(), pMessage->getData(), pMessage->getMaxLength() , 0,
		(struct sockaddr *)&clientAddr, &addr_len)) == -1) {
		perror("recvfrom");
		exit(1);
	}

    printf("Received a packet from a client of length %d bytes\n", numbytes);

    if(isVerbose())
    {
        dump_sockaddr_in((struct sockaddr_in *) &clientAddr);
    }

    /* Output that we got a packet from that client */
    pMessage->setType(pMessage->getData()[0]);

    /* Save the relevant info */
    pMessage->setLength((uint16_t) numbytes);
    pMessage->recordArrival();

    /* Copy / save the information about the client on the other side */
    memcpy(pMessage->getAddress(), &clientAddr, sizeof(struct sockaddr));

    if(isVerbose())
    {
        pMessage->dumpData();
    }

    return pMessage;
}

bool Tracker::processEcho (Message * pMessageEcho)
{
    Message * pEchoResponse;

    /* The inbound message has the following content:
        Type        1 byte      0x05   Echo
        Length      2 bytes     0x04   Four bytes of data to follow
        Nonce       4 bytes            Randomly generated value in network order
    */

    /* Our task is to reflect the nonce and add in a timestamp that is network
       ordered */

    printf("Processing an echo message from the client");

    uint32_t    theNonce;
    memcpy(&theNonce, pMessageEcho->getData()+3, 4);
    theNonce = ntohl(theNonce);

    printf("  Nonce (Network Order): %d\n", theNonce);

    pEchoResponse = new Message();

    pEchoResponse->setType(MSG_TYPE_ECHO_RESPONSE);
    pEchoResponse->setLength(16);

    // YOLO - Nothing can go wrong directly exposing a pointer and just letting the caller
    //        manipulate the byte array, right?

    /* Set the type */

    pEchoResponse->getData()[0] = MSG_TYPE_ECHO_RESPONSE;

    /* Set the size field */
    uint16_t    theSize;
    theSize = htons(pEchoResponse->getLength());
    memcpy(pEchoResponse->getData()+1, &theSize, 2);

    /* Set the status field - all is well */
    pEchoResponse->getData()[3] = 0;

    /* Copy over the 32 bit nonce data from the initial packet */
    memcpy(pEchoResponse->getData()+4, pMessageEcho->getData()+3, 4);

    /* Let's get the current time */
    struct timeval  theTimeVal;

    /* Who needs return codes? */
    gettimeofday(&theTimeVal, 0);

    printf("  The time at the server is %ld.%d\n", theTimeVal.tv_sec, theTimeVal.tv_usec);

    uint32_t theIntVal;
    theIntVal = htonl(theTimeVal.tv_sec);
    memcpy(pEchoResponse->getData()+8, &theIntVal, 4);

    theIntVal = htonl(theTimeVal.tv_usec);
    memcpy(pEchoResponse->getData()+12, &theIntVal, 4);

    printf("Sending a response from the server\n");
    pEchoResponse->dumpData();

    sendto(getSocket(), pEchoResponse->getData(), pEchoResponse->getLength(), 0, (struct sockaddr *) pMessageEcho->getAddress(), sizeof(struct sockaddr_in));

    return true;
}

bool Tracker::processRegister (Message * pMessageRegister)
{
    return false;
}

bool Tracker::processListNodes (Message * pMessageListNodes)
{
    return false;
}
