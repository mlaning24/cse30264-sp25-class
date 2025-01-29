
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

#include "Message.h"
#include "Client.h"
#include "Server.h"
#include "Thread_Output.h"

void * Thread_LobbyClient (void * pData);

GameClient::GameClient ()
{
    m_sName = "";
    m_nPosition = POSITION_UNKNOWN;
    m_pGame = NULL;
    m_State = CLIENT_STATE_UNKNOWN;

    m_bLobbyThreadRunning = false;

    m_pServer = NULL;
    m_pConnection = NULL;
}

GameClient::~GameClient ()
{

}

void GameClient::setName (string sName)
{
    m_sName = sName;
}

string GameClient::getName ()
{
    return m_sName;
}

void GameClient::setPosition (uint16_t nPos)
{
    m_nPosition = nPos;
}

uint16_t GameClient::getPosition ()
{
    return m_nPosition;
}

/* A thread for handling clients in the lobby */
void   GameClient::spawnThread_ClientLobby ()
{
    m_bLobbyThreadRunning = true;
    pthread_create(&m_ThreadLobby, NULL, Thread_LobbyClient, this);
}

bool GameClient::isLobbyThreadRunning ()
{
    return m_bLobbyThreadRunning;
}

GameClientConnection * GameClient::getConnection ()
{
    return m_pConnection;
}

void GameClient::setConnection (GameClientConnection * pConnection)
{
    m_pConnection = pConnection;
}



void  GameClient::setSocket_Lobby (int nSocketLobby)
{
    m_Socket_Lobby = nSocketLobby;
}

int GameClient::getSocket_Lobby ()
{
    return m_Socket_Lobby;
}

ClientState GameClient::getState ()
{
    return m_State;
}

void GameClient::setState (ClientState theState)
{
    m_State = theState;
}

void GameClient::setLobbyThreadRunning (bool bRunning)
{
    m_bLobbyThreadRunning = bRunning;
}

void GameClient::setServer (Server * pServer)
{
    m_pServer = pServer;
}

Server * GameClient::getServer ()
{
    return m_pServer;
}

string GameClient::getNonce ()
{
    return m_sNonce;
}

void GameClient::setNonce (string sNonce)
{
    m_sNonce = sNonce;
}


void * Thread_LobbyClient (void * pData)
{
    GameClient * pClient;
	char szBuffer[CLIENT_BUFFER_READ];
    char szOutput[CLIENT_BUFFER_READ];
	int	 numBytes;

    pClient = (GameClient *) pData;

    while(pClient->isLobbyThreadRunning())
    {
		if ((numBytes = recv(pClient->getSocket_Lobby(), szBuffer, CLIENT_BUFFER_READ-1, 0)) == -1) {
			perror("recv");
            pClient->setLobbyThreadRunning(false);
            pClient->setState(CLIENT_STATE_EXIT_LOBBY);
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

                GameMessageJoinResult   theResult;

                switch(pMessage->getType())
                {
                    case MESSAGETYPE_JOIN:
                        queueOutput("  Join message received\n");

                        GameMessageJoin *   pMessageJoin;

                        pMessageJoin = (GameMessageJoin *) pMessage;

                        pClient->setName(pMessageJoin->getName());

                        // Is this a valid client name?


                        // If it is, send a result message

                        theResult.setName(pClient->getName());
                        theResult.setResult("Yes");

                        memset(szBuffer, 0, CLIENT_BUFFER_READ);
                        sprintf(szBuffer, "%s", theResult.getJSON().c_str());

                        // Send the result back to them
                        send(pClient->getSocket_Lobby(), szBuffer, strlen(szBuffer), 0);

                        pClient->getServer()->addClient(pClient);                        
                        break;

                    case MESSAGETYPE_CHAT:
                        queueOutput("  Chat message received\n");

                        GameMessageChat *   pMessageChat;

                        pMessageChat = (GameMessageChat *) pMessage;

                        pClient->getServer()->chatLobby(pMessageChat);

                        break;

                    default:
                        queueOutput("Hmm - was not expecting that sort of message\n");
                        break;
                }     
            }                
            else 
            {
                queueOutput("No luck - unable to parse the message\n");
            }
        }
    }

    return NULL;
}


