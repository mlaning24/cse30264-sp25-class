

#ifndef __SERVER_H
#define __SERVER_H

#include <pthread.h>
#include <string>
#include <vector>
using namespace std;

#include <stdint.h>

class GameClient;
class GameInstance;
class GameMessageChat;


#define SERVER_PORT_LOBBY_DEFAULT       42000
#define SERVER_PORT_INSTANCE_DEFAULT    42500
#define SERVER_PORT_INSTANCES_MAX       5

#define SERVER_PORT_LOBBY_BACKLOG       10



#define SERVER_MAX_PLAYERS              4
#define SERVER_MAX_LOBBY_PORT           65535

#define DEFAULT_PLAYERS_PER_GAME        1
#define DEFAULT_ROUNDS_PER_GAME         3
#define DEFAULT_WORD_LENGTH_MIN         5
#define DEFAULT_WORD_LENGTH_MAX         5

#define DEFAULT_CHAT_ALL                true
#define DEFAULT_IGNORE_NONCE            false

class Server 
{
    private:
        // List of all clients for the system
        vector<GameClient *> m_Clients;

        // All established game instances
        vector<GameInstance *> m_Instances;

        // All available words
        vector<string> m_AvailableWords;
        

        uint16_t    m_nPort_Lobby;

        // What is the low end of game instance ports?
        uint16_t    m_nPort_InstanceLow;

        // How many instances can still be allocated?
        uint16_t    m_nInstances_Left;

        // What is the maximum number of instances?
        uint16_t    m_nInstances_Max;

        // The number of players per game
        uint16_t    m_nPlayersPerGame;

        // Rounds per game
        uint16_t    m_nRoundsPerGame;

        // Minimum length for words
        uint16_t    m_nWordLength_Min;

        // Maximum length for words
        uint16_t    m_nWordLength_Max;


        int        m_nListen_Backlog_Lobby;

        pthread_t   m_Thread_Lobby;

        bool        m_bKeepLobbyRunning;


        pthread_mutex_t     m_Mutex_ServerData;

        // Should chat messages go to everyone (including the sender)?
        bool        m_bChatAll;

        // Should we ignore the nonce for a join?
        bool        m_bIgnoreNonce;


        int         m_Socket_Lobby;

    public:
        Server ();
        ~Server ();

        /** Set the lobby port 
         * @param nLobbyPort The new port for the lobby 
         */
        void setPort_Lobby (uint16_t nLobbyPort);

        uint16_t getPort_Lobby ();

        string   getPort_Lobby_asString ();


        bool    start ();
        bool    stop ();

        bool    addClient (GameClient * pClient);

        bool    keepLobbyRunning ();

        void    chatLobby (GameMessageChat * pChat);

        int     getPlayersPerGame ();
        void    setPlayersPerGame (int nPlayers);

        bool    shouldChatAll ();
        void    setChatAll (bool bChatAll);

        bool    getIgnoreNonce ();
        void    setIgnoreNonce (bool bIgnore);

        uint16_t    getNextGamePort ();
        bool        isGamePortUsed (uint16_t nPort);

        int         getSocket_Lobby ();
        void        setSocket_Lobby (int nSocketLobby);
};

#endif