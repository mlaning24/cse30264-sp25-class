


#ifndef __GAME_INSTANCE_H
#define __GAME_INSTANCE_H

#include <vector>
#include <string>
using namespace std;

#include <pthread.h>

class GameClient;
class Server;
class GameInstance;
class GameMessageJoinInstance;

enum GameInstanceState
{
    GAMEINSTANCE_STATE_UNKNOWN,
    GAMEINSTANCE_STATE_START,
    GAMEINSTANCE_STATE_WAITING_CLIENTS,
    GAMEINSTANCE_STATE_STARTING_GAME,
    GAMEINSTANCE_STATE_STARTING_ROUND,
    GAMEINSTANCE_STATE_IN_ROUND,
    GAMEINSTANCE_STATE_END_ROUND,
    GAMEINSTANCE_STATE_END_GAME
};

#define INSTANCE_BUFFER_READ      1500


enum GameConnectionState
{
    GAMECONNECTION_STATE_JOIN_WAIT,     // Waiting for the join
    GAMECONNECTION_STATE_JOIN_RCVD,     // Received the join
    GAMECONNECTION_STATE_JOIN_REJECT,   // The join was rejected
    GAMECONNECTION_STATE_JOIN_ACCEPT,   // The join was accepted
    GAMECONNECTION_STATE_ACTIVE,        // The connection is active
    GAMECONNECTION_STATE_ERROR          // The connection went into an error state
};


class GameClientConnection 
{
    private:
        int             m_nSocket;    
        pthread_t       m_Thread;
        GameInstance *  m_pInstance;
        bool            m_bKeepRunning;
        GameConnectionState     m_State;

    public:
        GameClientConnection ();
        ~GameClientConnection ();

        int     getSocket ();
        void    setSocket (int nSocket);

        pthread_t   getThread ();
        void        setThread (pthread_t theThread);

        GameInstance *  getInstance ();
        void        setInstance (GameInstance * pInstance);

        bool        keepRunning ();
        void        setKeepRunning (bool bRunning);

        GameConnectionState     getState ();
        void                    setState (GameConnectionState theState);
};

/* A thread associated with a client instance */
void * Thread_ClientInstance (void * pData);



class GameInstance
{
    private:
        Server *       m_pServer;

        GameInstanceState       m_State;

        vector<GameClient *>    m_Clients;

        vector<GameClientConnection *> m_Connections;

        // Port number for the game
        uint16_t    m_nPort;

        // Which round are we on?
        uint16_t    m_nRound;


        // The socket for the game
        int         m_nSocket_Game;

        // Should this keep running?
        bool        m_bKeepRunning;

        pthread_mutex_t     m_Mutex;
        pthread_t           m_Thread;

        string      m_sNonce;

    public:
        GameInstance ();
        ~GameInstance ();

        uint16_t    getCount_Clients ();

        uint16_t    getCount_ConnectedClients ();

        uint16_t    getCount_GuessesReceived ();

        GameInstanceState   getState ();
        void                setState (GameInstanceState theState);

        void        addClient (GameClient * pClient);


        /** Get the port where the game instance is occurring 
         * @returns The port being used for the game
         */
        uint16_t    getPort_Game ();

        /** Set the port to use for this particular game 
         * @param nPort The port to use for the game 
         */
        void        setPort_Game (uint16_t nPort);

        /** Get the socket that the game instance is listening on 
         * @returns Valid socket that the game instance is listening on for clients 
         */
        int         getSocket ();

        /** Set the socket that the game instance is listening on
         * @param nSocket The socket that the instance is listening on
         */
        void        setSocket (int nSocket);


        void    setServer (Server * pServer);
        Server * getServer ();

        bool    start ();
        bool    stop ();

        bool    keepRunning ();

        bool    spawnClientThread (int nSocket);
        
        GameClient *    findClient (GameMessageJoinInstance * pJoin);

        
};

void * Thread_GameInstance (void * pData);


#endif