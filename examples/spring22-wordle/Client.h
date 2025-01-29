// Client.h : Header file defining a particular client connecting to
//            the multipler wordle game for Project 3
////////////////////////////////////////////////////////////////////////

#ifndef __CLIENT_H
#define __CLIENT_H

#include <string>
#include <vector>
using namespace std;

#include <time.h>
#include <stdint.h>
#include <pthread.h>

// Defintion only for GameInstance
class GameInstance;
class Server;
class GameClientConnection;

// Set of enumerations for the state of the client
enum ClientState 
{
    CLIENT_STATE_UNKNOWN,           // Unknown client state
    CLIENT_STATE_JOIN,              // Client has joined (post accept)
    CLIENT_STATE_REJECTED,          // Client was rejected  
    CLIENT_STATE_LOBBY,             // Client is in the lobby
    CLIENT_STATE_JOIN_INSTANCE,     // Client has been directed to an instance
    CLIENT_STATE_IN_INSTANCE,       // Client joined the instance
    CLIENT_STATE_IN_GAME,           // Client has been directed to be in a game
    CLIENT_STATE_IN_ROUND,          // Client is within a round
    CLIENT_STATE_DONE_GAME,         // Client has finished a game    
    CLIENT_STATE_EXIT_LOBBY         // Client exited early in the lobby
};

#define CLIENT_BUFFER_READ      1500
#define POSITION_UNKNOWN       10000

class GameClient
{
    private:
        string      m_sName;
        uint16_t    m_nPosition;


        ClientState     m_State;

        // Pointer to the game instance (if the player is in a game)
        GameInstance *  m_pGame;

        // Pointer to the server in case we need help
        Server       *  m_pServer;


        // Address of the client?
        // Port of the client (lobby)
        // Address of the client (game)
        // Port of the client (game)

        // Socket when the player is in the lobby
        int    m_Socket_Lobby;

        // Socket when the player is in the game
        int    m_Socket_Game;
        
        // Score of the client
        uint16_t    m_nScore;

        // Last guess of the client
        string      m_sGuess;

        // Timestamp of the last chat message from that client
        time_t      m_LastChatMesage;

        // Number of chat messages from this client
        uint16_t    m_nTx_Chat_Count;
        // Total length of the messages sent by the client
        uint32_t    m_nTx_Chat_Volume;

        // Number of chat messages sent to that client
        uint16_t    m_nRx_Chat_Count;

        // Total length of the messages sent to that client
        uint16_t    m_nRx_Chat_Volume; 

        // The pthread for the lobby
        pthread_t   m_ThreadLobby;

        bool        m_bLobbyThreadRunning;

        // Nonce for joining
        string      m_sNonce;

        // The connection if the client is in a game instance
        GameClientConnection *  m_pConnection;

    public:
        GameClient ();

        ~GameClient ();

        /** Set the name associated with this particular client / player
         * @param sName Name of the player
         */
        void setName (string sName);

        /** Get the name associated with this particular client / player
         * @returns A valid C++ string denoting the name of the player
         */
        string getName ();

        /** The position of the player in the game 
         * @param nPos The new position for the player
         */
        void setPosition (uint16_t nPos);

        /** Retrieve the position associated with this particular player
         * @returns The position of the player
         */
        uint16_t getPosition ();

        void    setSocket_Lobby (int nSocketLobby);
        int     getSocket_Lobby ();

        ClientState     getState ();
        void            setState (ClientState theState);

        Server *        getServer ();
        void            setServer (Server * pServer);


        /* A thread for handling clients in the lobby */
        void     spawnThread_ClientLobby ();

        bool     isLobbyThreadRunning ();
        void     setLobbyThreadRunning (bool bRunning);

        string      getNonce ();
        void        setNonce (string sNonce);

        GameClientConnection *    getConnection ();
        void                      setConnection (GameClientConnection * pConnection);

};

#endif
