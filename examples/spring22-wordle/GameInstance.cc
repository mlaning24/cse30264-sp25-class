
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

#include "GameInstance.h"
#include "Client.h"
#include "Server.h"
#include "Thread_Output.h"
#include "Message.h"
#include "SocketHelper.h"


GameClientConnection::GameClientConnection()
{
    m_nSocket = 0;
    m_pInstance = NULL;
    m_State = GAMECONNECTION_STATE_JOIN_WAIT;
}

GameClientConnection::~GameClientConnection ()
{

}

int GameClientConnection::getSocket ()
{
    return m_nSocket;
}

void GameClientConnection::setSocket (int nSocket)
{
    m_nSocket = nSocket;
}

pthread_t GameClientConnection::getThread ()
{
    return m_Thread;
}

void GameClientConnection::setThread (pthread_t theThread)
{
    m_Thread = theThread;
}

GameInstance * GameClientConnection::getInstance ()
{
    return m_pInstance;
}

void GameClientConnection::setInstance (GameInstance * pInstance)
{
    m_pInstance = pInstance;
}

bool GameClientConnection::keepRunning ()
{
    return m_bKeepRunning;
}

void GameClientConnection::setKeepRunning (bool bRunning)
{
    m_bKeepRunning = bRunning;
}

GameConnectionState GameClientConnection::getState ()
{
    return m_State;
}

void GameClientConnection::setState (GameConnectionState theState)
{
    m_State = theState;
}




GameInstance::GameInstance ()
{
    m_State = GAMEINSTANCE_STATE_UNKNOWN;
    m_pServer = NULL;
}

GameInstance::~GameInstance ()
{
    int j;

}

GameInstanceState GameInstance::getState ()
{
    return m_State;
}

void GameInstance::setState (GameInstanceState theState)
{
    m_State = theState;
}


uint16_t    GameInstance::getCount_Clients ()
{
    return 0;
}

uint16_t    GameInstance::getCount_ConnectedClients ()
{
    return 0;    
}

uint16_t    GameInstance::getCount_GuessesReceived ()
{
    return 0;
}

uint16_t    GameInstance::getPort_Game ()
{
    return m_nPort;
}

void        GameInstance::setPort_Game (uint16_t nPort)
{
    m_nPort = nPort;
}

void GameInstance::addClient (GameClient * pClient)
{
    m_Clients.push_back(pClient);
}

void    GameInstance::setServer (Server * pServer)
{
    m_pServer = pServer;
}

Server * GameInstance::getServer ()
{
    return m_pServer;
}

bool GameInstance::start ()
{
    // Initialize the mutex
    pthread_mutex_init(&m_Mutex, NULL);

    // Set it to keep running
    m_bKeepRunning = true;

    // Create / start the thread for handling inbound connections
    pthread_create(&m_Thread, NULL, Thread_GameInstance, this);

    return true;
}

bool GameInstance::stop ()
{
    m_bKeepRunning = false;

    return true;
}

bool GameInstance::keepRunning ()
{
    return m_bKeepRunning; 
}

GameClient * GameInstance::findClient (GameMessageJoinInstance * pJoin)
{
    char szBuffer[1000];

    pthread_mutex_lock(&m_Mutex);

    for(int j=0; j<m_Clients.size(); j++)
    {
        if(m_Clients[j]->getConnection() == NULL)
        {
            if(m_Clients[j]->getName() == pJoin->getName())
            {
                // Same name

                if(m_pServer->getIgnoreNonce() || 
                   m_Clients[j]->getNonce() == pJoin->getNonce())
                {
                    return m_Clients[j];
                }
                else if(m_Clients[j]->getNonce() != pJoin->getNonce()) 
                {
                    queueOutput("Warning: Client had a bad nonce value when trying to join\n");

                    sprintf(szBuffer, "  Expected %s (Len %ld)-> Received %s (Len=%ld)\n", m_Clients[j]->getNonce().c_str(), m_Clients[j]->getNonce().length(), pJoin->getNonce().c_str(), pJoin->getNonce().length());
                    queueOutput(szBuffer);
                    continue;
                }
            }
        }
        else
        {
            continue;
        }
    }

    pthread_mutex_unlock(&m_Mutex);
    return NULL;
}


void * Thread_GameInstance (void * pData)
{
	GameInstance * pInstance;
    int nSocketInstance;

	pInstance = (GameInstance *) pData;

    nSocketInstance = createSocket_TCP_Listen(pInstance->getPort_Game());
    pInstance->setSocket(nSocketInstance);
    

	// Adapting this from Beej's Guide
	
    int     new_fd;  // listen on sock_fd, new connection on new_fd

    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;

    char s[INET6_ADDRSTRLEN];
    int rv;
	
    char szOutputBuf[1000];

	queueOutput("Server (GameInstance): Waiting for connections .... \n");

    sprintf(szOutputBuf, "  Listening on port %d\n", pInstance->getPort_Game());
    queueOutput(szOutputBuf);

    while(pInstance->keepRunning()) 
	{  

        sin_size = sizeof their_addr;		
        new_fd = accept(pInstance->getSocket(), (struct sockaddr *)&their_addr, &sin_size);
		
        if (new_fd == -1) 
		{
            perror("accept");
            continue;
        }

        // TODO: Add in connection identification

		/* Simple bit of code but this can be helpful to detect successful
		   connections 
        inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof s);

		queueOutput("Server (Lobby): Got Connection from (");
		queueOutput(s);
		queueOutput("\n");
        */

       // At this point, what do we know?
       //
       //  Someone has connected to the game instance
       //  We don't know which this client this is associated with until
       //   they send us a JoinInstance message
       //
       //  Thus, we should make a thread to handle their data but basically
       //   restrict what they can do until they demonstrate they are a valid
       //   client (if we are rolling that way)


        pInstance->spawnClientThread(new_fd);
    }

    // The thread is exiting for the game instance. Close up the socket 
    // that this thread was monitoring
    close(pInstance->getSocket());

	return NULL;
}

void GameInstance::setSocket (int nSocket)
{
    m_nSocket_Game = nSocket;
}

int GameInstance::getSocket ()
{
    return m_nSocket_Game;
}

bool GameInstance::spawnClientThread (int nSocket)
{
    GameClientConnection    * pConnection;

    pConnection = new GameClientConnection();

    pConnection->setSocket(nSocket);
    pConnection->setInstance(this);
    pConnection->setKeepRunning(true);
    pConnection->setState(GAMECONNECTION_STATE_JOIN_WAIT);

    pthread_t   threadClient;
    pthread_create(&threadClient, NULL, Thread_ClientInstance, pConnection);

    // Add this to our list of potential connections
    pthread_mutex_lock(&m_Mutex);
    m_Connections.push_back(pConnection);
    pthread_mutex_unlock(&m_Mutex);

    return true;
}

void * Thread_ClientInstance (void * pData)
{
    char szBuffer[INSTANCE_BUFFER_READ];
    char szOutput[INSTANCE_BUFFER_READ];
	int	 numBytes;

    GameClientConnection * pConnection;

    pConnection = (GameClientConnection *) pData;

    while(pConnection->keepRunning())
    {
		if ((numBytes = recv(pConnection->getSocket(), szBuffer, INSTANCE_BUFFER_READ-1, 0)) == -1) {
			perror("recv");
            pConnection->setKeepRunning(false);
            return NULL;
		}

        if(numBytes > 0)
        {
            szBuffer[numBytes] = '\0';

            queueOutput("Message Received of ");
            sprintf(szOutput, "%d bytes\n", numBytes);
            queueOutput(szOutput);

            queueOutput("Message: ");
            queueOutput(szBuffer);
            queueOutput("\n");

            GameMessage * pMessage;

            pMessage = extractFromJSON(szBuffer);

            if(pMessage != NULL)
            {
                queueOutput("Message received and parsed - valid JSON\n");

                if(pConnection->getState() == GAMECONNECTION_STATE_JOIN_WAIT)
                {
                    if(pMessage->getType() != MESSAGETYPE_JOIN_INSTANCE)
                    {
                        if(pMessage->getType() == MESSAGETYPE_CHAT)
                        {
                            queueOutput("  Ignoring chat message - client has not yet joined the instance");
                            delete pMessage;
                            continue;
                        }
                        else
                        {
                            queueOutput("  Message was not a JoinInstance - ignoring");
                            delete pMessage;
                            continue;
                        }
                    }
                    else 
                    {
                        // Right message - will it match up
                        GameMessageJoinInstance * pJoinInstance;
                        GameClient * pClient;

                        pJoinInstance = (GameMessageJoinInstance *) pMessage;

                        pClient = pConnection->getInstance()->findClient(pJoinInstance);

                        if(pClient != NULL)
                        {
                            pClient->setConnection(pConnection);
                            pConnection->setState(GAMECONNECTION_STATE_JOIN_ACCEPT);
                            pClient->setState(CLIENT_STATE_IN_INSTANCE);
                            delete pMessage;

                            // Send a good JoinInstanceResult message
                        }
                        else
                        {
                            queueOutput("Error: Unable to join up this client - keeping the socket alive but not transitioning it into a valid state\n");
                            delete pMessage;

                            // Send a bad JoinInstanceResult message

                        }
                    }                        
                }
                else if(pConnection->getState() == GAMECONNECTION_STATE_JOIN_ACCEPT || 
                        pConnection->getState() == GAMECONNECTION_STATE_ACTIVE)
                {

                }
            }
        }
    }

    return NULL;
}


