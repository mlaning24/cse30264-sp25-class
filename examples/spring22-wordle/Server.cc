
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
#include <time.h> 

#include "Server.h"
#include "Thread_Lobby.h"
#include "Client.h"
#include "Message.h"
#include "Thread_Output.h"
#include "GameInstance.h"

Server::Server ()
{
    m_nPort_Lobby = SERVER_PORT_LOBBY_DEFAULT;
    m_nPort_InstanceLow = SERVER_PORT_INSTANCE_DEFAULT;

    m_nInstances_Max = SERVER_PORT_INSTANCE_DEFAULT;
    m_nInstances_Left = m_nInstances_Max;

    m_nPlayersPerGame = DEFAULT_PLAYERS_PER_GAME;
    m_nRoundsPerGame = DEFAULT_ROUNDS_PER_GAME;

    m_bKeepLobbyRunning = false;

    m_bChatAll = DEFAULT_CHAT_ALL;
    m_bIgnoreNonce = DEFAULT_IGNORE_NONCE;
}

Server::~Server ()
{

}


void Server::setPort_Lobby (uint16_t nPortLobby)
{
    m_nPort_Lobby = nPortLobby;
}

uint16_t Server::getPort_Lobby ()
{
    return m_nPort_Lobby;
}

string   Server::getPort_Lobby_asString ()
{
    char szBuffer[100];

    sprintf(szBuffer, "%d", m_nPort_Lobby);

    return string(szBuffer);
}


bool Server::start ()
{
    srand (time(NULL));

    /* Initialize the mutex for the server */
    pthread_mutex_init(&m_Mutex_ServerData, NULL);

    /* Start the lobby thread */
    m_bKeepLobbyRunning = true;
    pthread_create(&m_Thread_Lobby, NULL, Thread_Lobby, this);    

    return true;
}

bool Server::stop ()
{
    m_bKeepLobbyRunning = false;

    return true;
}

int Server::getPlayersPerGame()
{
    return m_nPlayersPerGame;
}

void Server::setPlayersPerGame (int nPlayers)
{
    m_nPlayersPerGame = nPlayers;
}

bool  Server::shouldChatAll ()
{
    return m_bChatAll;
}

void Server::setChatAll (bool bChatAll)
{
    m_bChatAll = bChatAll;
}

int Server::getSocket_Lobby ()
{
    return m_Socket_Lobby;
}

void Server::setSocket_Lobby (int nSocketLobby)
{
    m_Socket_Lobby = nSocketLobby;
}


bool Server::isGamePortUsed (uint16_t nPort)
{
    for(int j=0; j<m_Instances.size(); j++)
    {
        if(m_Instances[j]->getPort_Game() == nPort)
        {
            return true;
        }
    }

    return false;
}


uint16_t Server::getNextGamePort ()
{
    uint16_t    nTentativePort;

    // TODO: Need to add in logic that sorts instances by port number for this
    //       to work

    nTentativePort = m_nPort_InstanceLow;

    for(nTentativePort=m_nPort_InstanceLow; nTentativePort<=m_nPort_InstanceLow+m_nInstances_Max; nTentativePort++)
    {
        if(!isGamePortUsed(nTentativePort))
        {
            return nTentativePort;
        }
    }
    return 0;
}

bool    Server::getIgnoreNonce ()
{
    return m_bIgnoreNonce;
}

void    Server::setIgnoreNonce (bool bIgnore)
{
    m_bIgnoreNonce = bIgnore;
}



bool Server::addClient (GameClient * pClient)
{
    char szBuffer[1500];

    pthread_mutex_lock(&m_Mutex_ServerData);
    m_Clients.push_back(pClient);
    pthread_mutex_unlock(&m_Mutex_ServerData);

    /* Do we have enough clients? */
    pthread_mutex_lock(&m_Mutex_ServerData);

    sprintf(szBuffer, "Players in the lobby: %lu\n", m_Clients.size());
    queueOutput(szBuffer);

    if(m_Clients.size() == getPlayersPerGame())
    {
        sprintf(szBuffer, "Reached the number of players (N=%d) needed for a game\n", getPlayersPerGame());
        queueOutput(szBuffer);

        // Create a game instance with all of those players
        GameInstance    *   pInstance;

        pInstance = new GameInstance();

        // Assign the instance a port
        pInstance->setPort_Game(getNextGamePort());

        // Push this instance onto our "stack"
        m_Instances.push_back(pInstance);

        // Add all of the clients
        for(int j=0; j<m_Clients.size(); j++)
        {
            pInstance->addClient(m_Clients[j]);
        }

        // Start the game instance listening
        pInstance->setServer(this);
        pInstance->start();

        GameMessageStartInstance     theStartInstanceMessage;

        // Good old local host
        theStartInstanceMessage.setServerName("127.0.0.1");

        // TODO: Needs a way to see what is allocated and pick one
        theStartInstanceMessage.setPort(pInstance->getPort_Game());

        for(int j=0; j<m_Clients.size(); j++)
        {

            // Super complicated nonce
            sprintf(szBuffer, "%d", rand());
            m_Clients[j]->setNonce(szBuffer);
            theStartInstanceMessage.setNonce(szBuffer);

            sprintf(szBuffer, "%s", theStartInstanceMessage.getJSON().c_str());
            queueOutput("Message Sent: ");
            queueOutput(szBuffer);
            queueOutput("\n");
            send(m_Clients[j]->getSocket_Lobby(), szBuffer, strlen(szBuffer), 0);

            /* Set the client lobby thread to expire */
            m_Clients[j]->setLobbyThreadRunning(false);
        }

        /* Clear all of the clients while we still have the lock - they all went into the game */
        m_Clients.clear();        
    }
    
    pthread_mutex_unlock(&m_Mutex_ServerData);

    return true;
}

bool  Server::keepLobbyRunning ()
{
    return m_bKeepLobbyRunning;
}

void Server::chatLobby (GameMessageChat * pMessageChat)
{
    char szBuffer[1500];

    pthread_mutex_lock(&m_Mutex_ServerData);

    for(int j=0; j<m_Clients.size(); j++)
    {
        // There is an option to not send all message to all clients including the one 
        //  that sent it
        if(!shouldChatAll())
        {
            if(m_Clients[j]->getName() == pMessageChat->getName())
            {
                continue;
            }            
        }

        sprintf(szBuffer, "%s", pMessageChat->getJSON().c_str());
        send(m_Clients[j]->getSocket_Lobby(), szBuffer, strlen(szBuffer), 0);
    }

    pthread_mutex_unlock(&m_Mutex_ServerData);
}


