

#ifndef __MESSAGE_H
#define __MESSAGE_H

#include <string>
#include <vector>
#include <map>
using namespace std;

#include "include/rapidjson/document.h" 
#include "include/rapidjson/prettywriter.h"
using namespace rapidjson;


#include "PlayerInfo.h"

enum GameMessageType 
{
    MESSAGETYPE_UNKNOWN,
    MESSAGETYPE_JOIN,                   // 
    MESSAGETYPE_JOIN_RESULT,            //
    MESSAGETYPE_CHAT,                   //  
    MESSAGETYPE_START_INSTANCE,         //
    MESSAGETYPE_JOIN_INSTANCE,          //
    MESSAGETYPE_JOIN_INSTANCE_RESULT,   //
    MESSAGETYPE_START_GAME,             //
    MESSAGETYPE_START_ROUND,            //
    MESSAGETYPE_PROMPT_FOR_GUESS,       //
    MESSAGETYPE_GUESS,                  //
    MESSAGETYPE_GUESS_RESPONSE,         //
    MESSAGETYPE_GUESS_STATUS,           //
    MESSAGETYPE_GUESS_RESULT,           
    MESSAGETYPE_END_ROUND,              //
    MESSAGETYPE_END_GAME                //
};

class GameMessage 
{
    private:
        GameMessageType     m_Type;
        vector<PlayerInfo *>    m_PlayerInfo;

    public:
        GameMessage ();
        ~GameMessage ();

        GameMessage (GameMessageType theType);

        /** Retrieve the type of the message */
        GameMessageType     getType ();

        /** Set the type of the message */
        void                setType (GameMessageType theType);

        string              getType_AsString();

        /** Get the JSON representation of this message
         */
        string getJSON ();        

        /** Confirm that the JSON to be processed contains the right fields */
        virtual bool   confirmValidMessage (Document * pDoc);

        /** Add a copy of the PlayerInfo object to the internal vector 
         * containing player information 
         * @param pInfo A pointer to a valid piece of PlayerInfo
         */
        void        addAndCopyInfo  (PlayerInfo * pInfo);

        /** Add this PlayerInfo object to the internal vector 
         * containing player information. Note that the message will 
         * view itself as the proper owner of the pointer and will take
         * care of freeing up the pointer on descrition.   
         * @param pInfo A pointer to a valid piece of PlayerInfo
         */
        void        addInfo         (PlayerInfo * pInfo);

        /** Iterate through the list of player information and free up
         * all pointers if any pointers are present
         */        
        void        clearInfo       ();



    protected:
        /** Retrieve the data */
        virtual string getJSON_Data();

        /** Confirm that the JSON to be processed contains the right fields */
        bool   confirmValidMessage_Data     (Document * pDoc, map<string, string> * pMap);

        /** Confirm that the JSON to be processed contains the right fields in the PlayerInfo sub-array */
        bool   confirmValidMessage_PlayerInfo (Document * pDoc, map<string, string> * pMap);

        vector<PlayerInfo *> *  getPlayerInfo ();

};

/** The message sent from the client to the server to initially join
 */
class GameMessageJoin : public GameMessage
{
    private:
        string      m_sName;
        string      m_sClientInfo;

    public:
        GameMessageJoin ();

        void        setName         (string sName);
        void        setClientInfo   (string sClientInfo);

        string      getName ();
        string      getClientInfo ();

    protected:
        virtual string getJSON_Data();

};

/** The message sent from the server to the client in response to a join
 */
class GameMessageJoinResult : public GameMessage
{
    private:
        string      m_sName;
        string      m_sResult;

    public:
        GameMessageJoinResult ();

        void        setName         (string sName);
        void        setResult       (string sResult);

        string      getName ();
        string      getResult ();

    protected:
        virtual string getJSON_Data();
};

/** A chat message that can go to / from the client or the server
 */
class GameMessageChat : public GameMessage
{
    private:
        string      m_sName;
        string      m_sChat;

    public: 
        GameMessageChat ();

        void        setName     (string sName);
        string      getName     ();

        void        setChat     (string sChat);
        string      getChat     (); 

    protected:
        virtual string      getJSON_Data();
};

/** The StartInstance message is sent by the server a client telling the
 *  client where the game instance will be (port-wise) 
 */
class GameMessageStartInstance : public GameMessage 
{
    private:
        string      m_sServerName;
        string      m_sPort;
        string      m_sNonce;

    public: 
        GameMessageStartInstance ();

        void        setServerName     (string sName);
        string      getServerName     ();

        void        setPort     (string sPort);
        void        setPort     (uint16_t nPort);
        string      getPort     ();

        void        setNonce    (int nNonce);
        void        setNonce    (string sNonce);
        string      getNonce    (); 

    protected:
        virtual string getJSON_Data();

};


/** The JoinInstance message is sent by a client to join a game instance 
 */
class GameMessageJoinInstance : public GameMessage 
{
    private:
        string      m_sName;
        string      m_sNonce;

    public: 
        GameMessageJoinInstance ();

        void        setName     (string sName);
        string      getName     ();

        void        setNonce    (int nNonce);
        void        setNonce     (string sChat);
        string      getNonce     (); 

    protected:
        virtual string getJSON_Data();

};

/** The JoinInstanceResult message is sent by a server in response to join a game instance 
 */
class GameMessageJoinInstanceResult : public GameMessage 
{
    private:
        string      m_sName;
        int         m_nNumber;
        string      m_sResult;

    public: 
        GameMessageJoinInstanceResult ();

        void        setName     (string sName);
        string      getName     ();

        void        setNumber     (int nNumber);
        int         getNumber     (); 

        void        setResult     (string sResult);
        string      getResult     (); 

    protected:
        virtual string getJSON_Data();

};

/** The StartGame message is sent by a server to note the game parameters 
 */
class GameMessageStartGame : public GameMessage 
{
    private:
        int         m_nRounds;

    public: 
        GameMessageStartGame ();
        ~GameMessageStartGame ();

        void        setRounds     (int nNumber);
        int         getRounds     (); 

    protected:
        virtual string getJSON_Data();

};

/** The StartRound message is sent by a server to note the round parameters 
 */
class GameMessageStartRound : public GameMessage 
{
    private:
        int         m_nRounds;
        int         m_nRoundsRemaining;
        int         m_nLength;

        vector<PlayerInfo *>    m_PlayerInfo;


    public: 
        GameMessageStartRound  ();
        ~GameMessageStartRound ();

        void        setLength     (int nNumber);
        int         getLength     (); 
        string      getLengthAsString ();


        void        setRounds     (int nNumber);
        int         getRounds     (); 
        string      getRoundsAsString ();

        void        setRoundsRemaining (int nNumber);
        int         getRoundsRemaining (); 
        string      getRoundsRemainingAsString ();

    protected:
        virtual string getJSON_Data();

};



/** The GameMessagePromptForGuess message is sent by the server to each client
 * telling them to make a guess
 */
class GameMessagePromptForGuess : public GameMessage 
{
    private:
        int     m_nWordLength;
        string  m_sName;
        int     m_nGuessNumber;

    public: 
        GameMessagePromptForGuess ();

        void    setWordLength (int nLength);
        int     getWordLength ();
        string  getWordLengthAsString ();

        void    setName (string sName);
        string  getName ();

        void    setGuessNumber (int nNumber);
        int     getGuessNumber ();
        string  getGuessNumberAsString ();

        /** Confirm that the JSON to be processed contains the right fields */
        virtual bool   confirmValidMessage (Document * pDoc);

    protected:
        virtual string getJSON_Data();

};


/** Message sent from the client to the server for a guess
 */
class GameMessageGuess : public GameMessage
{
    private:
        string      m_sName;
        string      m_sGuess;

    public:
        GameMessageGuess ();

        void        setName    (string sName);
        void        setGuess   (string sGuess);

        string      getName     ();
        string      getGuess    ();

    protected:
        virtual string getJSON_Data();

};

/** Message sent from the client to the server for a guess
 */
class GameMessageGuessResponse : public GameMessage
{
    private:
        string      m_sName;
        string      m_sGuess;
        string      m_sAccepted;

    public:
        GameMessageGuessResponse ();

        void        setName    (string sName);
        void        setGuess   (string sGuess);
        void        setAccepted   (string sAccepted);

        string      getName     ();
        string      getGuess    ();
        string      getAccepted ();

    protected:
        virtual string getJSON_Data();
};

/** Message sent from the client to the server to force a guess result
 */
class GameMessageGuessStatus : public GameMessage
{
    private:
        // No data

    public:
        GameMessageGuessStatus ();

        /** Confirm that the JSON to be processed contains the right fields */
        virtual bool   confirmValidMessage (Document * pDoc);

    protected:
        virtual string getJSON_Data();

};

/** The GuessResult message is sent once all players have guessed or 
 * as prompted by a GuessStatus message from a player. It contains all
 * player information and information about their guess from a given
 * round.
 */
class GameMessageGuessResult : public GameMessage 
{
    // Sent once everyone has guessed who is a part of the game
    //
    // Winner - Yes or No denoting if someone guessed correctly
    // An array of player information (PlayerInfo) including
    //  Name - Player name
    //  Number - Player number
    //  Correct - Yes or No if the entire guess was correct  
    //  ReceiptTime - Time in UTC including seconds and microseconds
    //  Result - String containing a set of B, Y, G which refer to black (B - wrong guess), yellow (Y - right letter, wrong spot), and green (G - right letter, right spot)    

    private:
        bool        m_bGuessCorrect      ;
        vector<PlayerInfo *>    m_PlayerInfo;

    public: 
        GameMessageGuessResult ();
        ~GameMessageGuessResult ();

        void    setGuessCorrect (string sCorrect);
        void    setGuessCorrect (bool bCorrect);
        bool    getGuessCorrect ();
        string  getGuessCorrectAsString ();

        /** Confirm that the JSON to be processed contains the right fields */
        virtual bool   confirmValidMessage (Document * pDoc);

    protected:
        virtual string getJSON_Data();
};

/** The EndRound message is sent by a server to note the end of a round
 */
class GameMessageEndRound : public GameMessage 
{
    private:
        int         m_nRoundsRemaining;
        vector<PlayerInfo *>    m_PlayerInfo;

    public: 
        GameMessageEndRound ();
        ~GameMessageEndRound ();

        void        setRoundsRemaining (int nNumber);
        int         getRoundsRemaining (); 
        string      getRoundsRemainingAsString ();

        /** Confirm that the JSON to be processed contains the right fields */
        virtual bool   confirmValidMessage (Document * pDoc);

    protected:
        virtual string getJSON_Data();

};


/** The EndGame message is sent by a server to note the end of the game
 */
class GameMessageEndGame : public GameMessage 
{
    private:
        string         m_sWinner;
        vector<PlayerInfo *>    m_PlayerInfo;


    public: 
        GameMessageEndGame ();
        ~GameMessageEndGame ();

        void        setWinner     (string sWinner);
        string      getWinner     (); 

        /** Confirm that the JSON to be processed contains the right fields */
        virtual bool   confirmValidMessage (Document * pDoc);

    protected:
        virtual string getJSON_Data();
};



/** Extract a GameMessage from JSON message
 */
GameMessage * extractFromJSON (string theMessage);


/** Test suite */
void testsuite_Message ();

#endif
