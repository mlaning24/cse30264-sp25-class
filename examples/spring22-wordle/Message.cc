
#include <time.h>
#include <sys/time.h>

#include "Message.h"

#include "include/rapidjson/document.h" 
#include "include/rapidjson/prettywriter.h"
using namespace rapidjson;

#include <iostream>
#include <iterator>
using namespace std;

#include "Thread_Output.h"

GameMessage::GameMessage ()
{
    m_Type = MESSAGETYPE_UNKNOWN;
}

GameMessage::GameMessage (GameMessageType theType)
{
    m_Type = theType;
}

GameMessage::~GameMessage ()
{
    clearInfo();
}

void   GameMessage::addAndCopyInfo     (PlayerInfo * pInfo)
{
    PlayerInfo *    pCopiedInfo;

    pCopiedInfo = new PlayerInfo();

    pCopiedInfo->setName(pInfo->getName());
    pCopiedInfo->setNumber(pInfo->getNumber());
    pCopiedInfo->setReceiptTime(pInfo->getReceiptTime());
    pCopiedInfo->setResult(pInfo->getResult());
    pCopiedInfo->setScore(pInfo->getScore());
    pCopiedInfo->setWinner(pInfo->getWinner());

    m_PlayerInfo.push_back(pCopiedInfo);
}

void   GameMessage::addInfo             (PlayerInfo * pInfo)
{
    m_PlayerInfo.push_back(pInfo);
}

void   GameMessage::clearInfo  ()
{
    while(m_PlayerInfo.size() > 0)
    {
        PlayerInfo  *   pFirst;

        pFirst = m_PlayerInfo[m_PlayerInfo.size()-1];
        delete pFirst;
        m_PlayerInfo.pop_back();
    }    
}

vector<PlayerInfo *> * GameMessage::getPlayerInfo ()
{
    return &m_PlayerInfo;
}

void GameMessage::setType (GameMessageType theType)
{
    m_Type = theType;
}

GameMessageType GameMessage::getType ()
{
    return m_Type;
}

string GameMessage::getType_AsString ()
{
    switch(m_Type)
    {
        case MESSAGETYPE_UNKNOWN:
            return "Unknown";

        case MESSAGETYPE_JOIN:
            return "Join";

        case MESSAGETYPE_JOIN_RESULT:
            return "JoinResult";

        case MESSAGETYPE_CHAT:
            return "Chat";

        case MESSAGETYPE_START_INSTANCE:
            return "StartInstance";

        case MESSAGETYPE_JOIN_INSTANCE:
            return "JoinInstance";

        case MESSAGETYPE_JOIN_INSTANCE_RESULT:
            return "JoinInstanceResult";

        case MESSAGETYPE_START_GAME:
            return "StartGame";

        case MESSAGETYPE_START_ROUND:
            return "StartRound";

        case MESSAGETYPE_PROMPT_FOR_GUESS:
            return "PromptForGuess";

        case MESSAGETYPE_GUESS:
            return "Guess";

        case MESSAGETYPE_GUESS_RESPONSE:
            return "GuessResponse";

        case MESSAGETYPE_GUESS_STATUS:
            return "GuessStatus";

        case MESSAGETYPE_GUESS_RESULT:
            return "GuessResult";

        case MESSAGETYPE_END_ROUND:
            return "EndRound";

        case MESSAGETYPE_END_GAME:
            return "EndGame";           

        default:
            return "";
    }

    return "";
}

string GameMessage::getJSON ()
{
    string sMessage;

    sMessage = "{ \"MessageType\" : \"" + getType_AsString() + "\", \"Data\" : ";
    sMessage += getJSON_Data();
    sMessage += " }";

    return sMessage;
}

string GameMessage::getJSON_Data ()
{
    return "";
}

bool GameMessage::confirmValidMessage (Document * pDoc)
{
    return false;
}

/** Confirm that the JSON to be processed contains the right fields in the PlayerInfo sub-array */
bool  GameMessage::confirmValidMessage_PlayerInfo (Document * pDoc, map<string, string> * pMap)
{
    Value & InfoMember = (*pDoc)["Data"]["PlayerInfo"];
    bool bSuccess;

    map<string, string>::iterator it = pMap->begin();


    for (SizeType i = 0; i < InfoMember.Size(); i++)   
    {     
        const Value & PlayerMember = InfoMember[i];

        for(it = pMap->begin(); it != pMap->end(); ++it)         
        {
            bSuccess = false;

            for(Value::ConstMemberIterator itrI = PlayerMember.MemberBegin(); itrI != PlayerMember.MemberEnd(); ++itrI)
            {
                auto & member_name = itrI->name;
                auto & value = itrI->value;

                if(member_name.GetString() == it->first)
                {
                    if(it->second == "string")
                    {
                        if(value.IsString())
                        {
                            bSuccess = true;
                            break;
                        }
                        else
                        {
                            cerr << "Error: The " << getType_AsString() << " message member " << it->first << " is not a " << it->second << endl;                        
                            return false;
                        }
                    }
                    else if(it->second == "number")
                    {
                        if(value.IsNumber())
                        {
                            bSuccess = true;
                            break;
                        }
                        else
                        {
                            cerr << "Error: The " << getType_AsString() << " message member " << it->first << " is not a " << it->second << endl;                        
                            return false;
                        }
                    }
                    else if(it->second == "array")
                    {
                        if(value.IsArray())
                        {
                            bSuccess = true;
                            break;
                        }
                        else
                        {
                            cerr << "Error: The " << getType_AsString() << " message member " << it->first << " is not a " << it->second << endl;                        
                            return false;
                        }
                    }
                }
            }

            // Did we find everything we needed?
            if(!bSuccess)
            {
                // We got here and did not find the expected membership
                cerr << "Error: The " << getType_AsString() << " message does not have a " << it->first << " member in array entry " << i << endl;
                return false;
            }

        }        
    }

    return true;
}


bool GameMessage::confirmValidMessage_Data     (Document * pDoc, map<string, string> * pMap)
{
    Value & DataMember = (*pDoc)["Data"];

    map<string, string>::iterator it = pMap->begin();

    bool bSuccess;

    while(it != pMap->end())
    {
        bSuccess = false;

        for(Value::ConstMemberIterator itr = DataMember.MemberBegin(); itr != DataMember.MemberEnd(); ++itr)
        {
            auto & member_name = itr->name;
            auto & value = itr->value;

            if(member_name.GetString() == it->first)
            {
                if(it->second == "string")
                {
                    if(value.IsString())
                    {
                        bSuccess = true;
                        break;
                    }
                    else
                    {
                        cerr << "Error: The " << getType_AsString() << " message member " << it->first << " is not a " << it->second << endl;                        
                        return false;
                    }
                }
                else if(it->second == "number")
                {
                    if(value.IsNumber())
                    {
                        bSuccess = true;
                        break;
                    }
                    else
                    {
                        cerr << "Error: The " << getType_AsString() << " message member " << it->first << " is not a " << it->second << endl;                        
                        return false;
                    }
                }
                else if(it->second == "array")
                {
                    if(value.IsArray())
                    {
                        bSuccess = true;
                        break;
                    }
                    else
                    {
                        cerr << "Error: The " << getType_AsString() << " message member " << it->first << " is not a " << it->second << endl;                        
                        return false;
                    }
                }

            }            
        }

        if(!bSuccess)
        {
            // We got here and did not find the expected membership
            cerr << "Error: The " << getType_AsString() << " message does not have a " << it->first << " member" << endl;
            return false;
        }

        it++;
    }

    return true;
}

//////////////////////////////////////////////////////////////////////

GameMessageJoin::GameMessageJoin ()
{
    setType(MESSAGETYPE_JOIN);
    m_sName = "";
    m_sClientInfo = "";
}

void GameMessageJoin::setName (string theName)
{
    m_sName = theName;
}


void GameMessageJoin::setClientInfo (string theInfo)
{
    m_sClientInfo = theInfo;
}

string GameMessageJoin::getName ()
{
    return m_sName;
}

string GameMessageJoin::getClientInfo ()
{
    return m_sClientInfo;
}

string GameMessageJoin::getJSON_Data ()
{
    return "{ \"Name\" : \"" + getName() + "\", \"Client\" : \"" + getClientInfo() + "\" }";
}


//////////////////////////////////////////////////////////////////////

GameMessageJoinResult::GameMessageJoinResult ()
{
    setType(MESSAGETYPE_JOIN_RESULT);
    m_sName = "";
    m_sResult = "";
}

void GameMessageJoinResult::setName (string theName)
{
    m_sName = theName;
}


void GameMessageJoinResult::setResult (string sResult)
{
    m_sResult = sResult;
}

string GameMessageJoinResult::getName ()
{
    return m_sName;
}

string GameMessageJoinResult::getResult ()
{
    return m_sResult;
}

string GameMessageJoinResult::getJSON_Data ()
{
    return "{ \"Name\" : \"" + getName() + "\", \"Result\" : \"" + getResult() + "\" }";
}


///////////////////////////////////////////////////////////////

GameMessageChat::GameMessageChat ()
{
    setType(MESSAGETYPE_CHAT);
    m_sName = "";
    m_sChat = "";
}

void GameMessageChat::setName (string theName)
{
    m_sName = theName;
}


void GameMessageChat::setChat (string theChat)
{
    m_sChat = theChat;
}

string GameMessageChat::getName ()
{
    return m_sName;
}

string GameMessageChat::getChat ()
{
    return m_sChat;
}

string GameMessageChat::getJSON_Data ()
{
    string  sMessage;

    sMessage = "{ \"Name\" : \"" + m_sName + "\", \"Chat\" : \"" + m_sChat + "\" }"; 

    return sMessage;
}

///////////////////////////////////////////////////////////////

GameMessageStartInstance::GameMessageStartInstance ()
{
    setType(MESSAGETYPE_START_INSTANCE);
    m_sServerName = "";
    m_sPort = "";
    m_sNonce = "";
}

void GameMessageStartInstance::setServerName (string theName)
{
    m_sServerName = theName;
}

void GameMessageStartInstance::setNonce (string theNonce)
{
    m_sNonce = theNonce;
}

void GameMessageStartInstance::setNonce (int nNonce)
{
    char szNonceString [20];

    sprintf(szNonceString, "%d", nNonce);

    m_sNonce = szNonceString;
}


void GameMessageStartInstance::setPort (string sPort)
{
    m_sPort = sPort;
}

void GameMessageStartInstance::setPort (uint16_t nPort)
{
    char szPortString [10];

    sprintf(szPortString, "%d", nPort);

    m_sPort = szPortString;
}


string GameMessageStartInstance::getServerName ()
{
    return m_sServerName;
}

string GameMessageStartInstance::getNonce ()
{
    return m_sNonce;
}

string GameMessageStartInstance::getPort ()
{
    return m_sPort;
}

string GameMessageStartInstance::getJSON_Data ()
{
    string  sMessage;

    sMessage = "{ \"Server\" : \"" + m_sServerName + "\", \"Port\" : " + m_sPort + ", \"Nonce\" : " + m_sNonce + " }"; 
    return sMessage;
}



///////////////////////////////////////////////////////////////

GameMessageJoinInstance::GameMessageJoinInstance ()
{
    setType(MESSAGETYPE_JOIN_INSTANCE);
    m_sName = "";
    m_sNonce = "";
}

void GameMessageJoinInstance::setName (string theName)
{
    m_sName = theName;
}

void GameMessageJoinInstance::setNonce (int nNonce)
{
    char szNonceString [20];

    sprintf(szNonceString, "%d", nNonce);

    m_sNonce = szNonceString;
}


void GameMessageJoinInstance::setNonce (string theNonce)
{
    m_sNonce = theNonce;
}

string GameMessageJoinInstance::getName ()
{
    return m_sName;
}

string GameMessageJoinInstance::getNonce ()
{
    return m_sNonce;
}

string GameMessageJoinInstance::getJSON_Data ()
{
    string  sMessage;

    sMessage = "{ \"Name\" : \"" + m_sName + "\", \"Nonce\" : " + m_sNonce + " }"; 
    return sMessage;
}

///////////////////////////////////////////////////////////////

GameMessageJoinInstanceResult::GameMessageJoinInstanceResult ()
{
    setType(MESSAGETYPE_JOIN_INSTANCE_RESULT);
    m_sName = "";
    m_nNumber = -1;
    m_sResult = "";
}

void GameMessageJoinInstanceResult::setName (string theName)
{
    m_sName = theName;
}

string GameMessageJoinInstanceResult::getName ()
{
    return m_sName;
}

int GameMessageJoinInstanceResult::getNumber ()
{
    return m_nNumber;
}

void GameMessageJoinInstanceResult::setNumber (int nNumber)
{
    m_nNumber = nNumber;
}

string GameMessageJoinInstanceResult::getResult ()
{
    return m_sResult;
}

void GameMessageJoinInstanceResult::setResult (string sResult)
{
    m_sResult = sResult;
}

string GameMessageJoinInstanceResult::getJSON_Data ()
{
    string  sMessage;
    char    szNumber[20];

    sprintf(szNumber, "%d", getNumber());

    sMessage = "{ \"Name\" : \"" + m_sName + "\", \"Number\" : " + szNumber + ", \"Result\" : \"" + m_sResult + "\" }"; 
    return sMessage;
}

///////////////////////////////////////////////////////////////

GameMessageStartGame::GameMessageStartGame ()
{
    setType(MESSAGETYPE_START_GAME);
    m_nRounds = 0;
}

GameMessageStartGame::~GameMessageStartGame ()
{
    clearInfo();
}

int GameMessageStartGame::getRounds ()
{
    return m_nRounds;
}

void GameMessageStartGame::setRounds (int nRounds)
{
    m_nRounds = nRounds;
}

string GameMessageStartGame::getJSON_Data ()
{
    string  sMessage;
    char    szNumber[20];

    sprintf(szNumber, "%d", getRounds());

    sMessage = "{ \"Rounds\" : " + (string) szNumber + ", \"PlayerInfo\" : ["; 

    for(int j=0; j<getPlayerInfo()->size(); j++)
    {
        sMessage += "{";
        sMessage += "\"Name\" : \"" + getPlayerInfo()->at(j)->getName() + "\", ";
        sMessage += "\"Number\" : " + getPlayerInfo()->at(j)->getNumberAsString() + ", ";
        sMessage += "\"Score\" : " + getPlayerInfo()->at(j)->getScoreAsString() + " ";
        sMessage += "}";

        if(j+1 < getPlayerInfo()->size())
        {
            sMessage += ", ";
        }
    }

    sMessage += "] ";
    sMessage += "}";

    return sMessage;
}

///////////////////////////////////////////////////////////////

GameMessageStartRound::GameMessageStartRound ()
{
    setType(MESSAGETYPE_START_ROUND);
    m_nRounds = 0;
    m_nRoundsRemaining = 0;
    m_nLength = 0;
}

GameMessageStartRound::~GameMessageStartRound ()
{
    clearInfo();
}

int GameMessageStartRound::getRounds ()
{
    return m_nRounds;
}

string GameMessageStartRound::getRoundsAsString ()
{
    char szBuffer[20];

    sprintf(szBuffer, "%d", m_nRounds);

    return szBuffer;
}

void GameMessageStartRound::setRounds (int nRounds)
{
    m_nRounds = nRounds;
}

int GameMessageStartRound::getRoundsRemaining ()
{
    return m_nRoundsRemaining;
}

string GameMessageStartRound::getRoundsRemainingAsString ()
{
    char szBuffer[20];

    sprintf(szBuffer, "%d", m_nRoundsRemaining);

    return szBuffer;
}

void GameMessageStartRound::setRoundsRemaining (int nRounds)
{
    m_nRoundsRemaining = nRounds;
}

int GameMessageStartRound::getLength ()
{
    return m_nLength;
}

string GameMessageStartRound::getLengthAsString ()
{
    char szBuffer[20];

    sprintf(szBuffer, "%d", m_nLength);

    return szBuffer;
}

void GameMessageStartRound::setLength (int nLength)
{
    m_nLength = nLength;
}

string GameMessageStartRound::getJSON_Data ()
{
    string  sMessage;

    sMessage = "{ \"Rounds\" : " + getRoundsAsString() + ", ";
    sMessage += "\"RoundsRemaining\" : " + getRoundsRemainingAsString() + ", ";
    sMessage += "\"Length\" : " + getLengthAsString() + ", ";

    // Include the player information    
    sMessage += "\"PlayerInfo\" : ["; 

    for(int j=0; j<getPlayerInfo()->size(); j++)
    {
        sMessage += "{";
        sMessage += "\"Name\" : \"" + getPlayerInfo()->at(j)->getName() + "\", ";
        sMessage += "\"Number\" : " + getPlayerInfo()->at(j)->getNumberAsString() + ", ";
        sMessage += "\"Score\" : " + getPlayerInfo()->at(j)->getScoreAsString() + " ";
        sMessage += "}";

        if(j+1 < getPlayerInfo()->size())
        {
            sMessage += ", ";
        }
    }

    sMessage += "] ";
    sMessage += "}";

    return sMessage;
}

///////////////////////////////////////////////////////////////

GameMessagePromptForGuess::GameMessagePromptForGuess ()
{
    setType(MESSAGETYPE_PROMPT_FOR_GUESS);
    m_nWordLength = 0;
    m_sName = "";
    m_nGuessNumber = 0;
}

void    GameMessagePromptForGuess::setWordLength (int nLength)
{
    m_nWordLength = nLength;
}

int     GameMessagePromptForGuess::getWordLength ()
{
    return m_nWordLength;
}

string  GameMessagePromptForGuess::getWordLengthAsString ()
{
    char    szBuffer[20];

    sprintf(szBuffer, "%d", m_nWordLength);
    return szBuffer;
}

void    GameMessagePromptForGuess::setName (string sName)
{
    m_sName = sName;
}

string  GameMessagePromptForGuess::getName ()
{
    return m_sName;
}

void    GameMessagePromptForGuess::setGuessNumber (int nNumber)
{
    m_nGuessNumber = nNumber;
}

int     GameMessagePromptForGuess::getGuessNumber ()
{
    return m_nGuessNumber;
}

string  GameMessagePromptForGuess::getGuessNumberAsString ()
{
    char    szBuffer[20];

    sprintf(szBuffer, "%d", m_nGuessNumber);
    return szBuffer;    
}

bool GameMessagePromptForGuess::confirmValidMessage (Document * pDoc)
{
    map<string, string>     theMap;

    theMap.insert(pair<string, string>("WordLength", "number"));
    theMap.insert(pair<string, string>("Name", "string"));
    theMap.insert(pair<string, string>("GuessNumber", "number"));

    if(!confirmValidMessage_Data(pDoc, &theMap))
        return false;

    return true;
}

string GameMessagePromptForGuess::getJSON_Data ()
{
    string  sMessage;

    sMessage =   "{ \"WordLength\" : " + getWordLengthAsString() + ", \"Name\" : \"" + m_sName + "\", ";
    sMessage +=  "\"GuessNumber\" : " + getGuessNumberAsString() + " }"; 

    return sMessage;
}


///////////////////////////////////////////////////////////////

GameMessageGuess::GameMessageGuess ()
{
    setType(MESSAGETYPE_GUESS);
    m_sName = "";
    m_sGuess = "";
}

void GameMessageGuess::setName (string theName)
{
    m_sName = theName;
}


void GameMessageGuess::setGuess (string theGuess)
{
    m_sGuess = theGuess;
}

string GameMessageGuess::getName ()
{
    return m_sName;
}

string GameMessageGuess::getGuess ()
{
    return m_sGuess;
}

string GameMessageGuess::getJSON_Data ()
{
    string  sMessage;

    sMessage = "{ \"Name\" : \"" + m_sName + "\", \"Guess\" : \"" + m_sGuess + "\" }"; 

    return sMessage;
}

///////////////////////////////////////////////////////////////

GameMessageGuessResponse::GameMessageGuessResponse ()
{
    setType(MESSAGETYPE_GUESS_RESPONSE);
    m_sName = "";
    m_sGuess = "";
    m_sAccepted = "";
}

void GameMessageGuessResponse::setName (string theName)
{
    m_sName = theName;
}

void GameMessageGuessResponse::setGuess (string theGuess)
{
    m_sGuess = theGuess;
}

void GameMessageGuessResponse::setAccepted (string sAccepted)
{
    m_sAccepted = sAccepted;
}

string GameMessageGuessResponse::getName ()
{
    return m_sName;
}

string GameMessageGuessResponse::getGuess ()
{
    return m_sGuess;
}

string GameMessageGuessResponse::getAccepted ()
{
    return m_sAccepted;
}


string GameMessageGuessResponse::getJSON_Data ()
{
    string  sMessage;

    sMessage = "{ \"Name\" : \"" + m_sName + "\", \"Guess\" : \"" + m_sGuess + "\", \"Accepted\" : \"" + m_sAccepted + "\"  }"; 
    return sMessage;
}

///////////////////////////////////////////////////////////////

GameMessageGuessStatus::GameMessageGuessStatus ()
{
    setType(MESSAGETYPE_GUESS_STATUS);
}

string GameMessageGuessStatus::getJSON_Data ()
{
    string  sMessage;

    // Nothing for the data
    sMessage = "{ }"; 

    return sMessage;
}

/** Confirm that the JSON to be processed contains the right fields */
bool   GameMessageGuessStatus::confirmValidMessage (Document * pDoc)
{
    // Nothing really to check
    return true;
}

///////////////////////////////////////////////////////////////

GameMessageGuessResult::GameMessageGuessResult ()
{
    setType(MESSAGETYPE_GUESS_RESULT);
    m_bGuessCorrect = false;
}

GameMessageGuessResult::~GameMessageGuessResult ()
{
    clearInfo();
}

void    GameMessageGuessResult::setGuessCorrect (string sCorrect)
{
    if(sCorrect == "Yes")
    {
        setGuessCorrect(true);
    }
    else
    {
        setGuessCorrect(false);
    }
}

void    GameMessageGuessResult::setGuessCorrect (bool bCorrect)
{
    m_bGuessCorrect = bCorrect;
}

bool    GameMessageGuessResult::getGuessCorrect ()
{
    return m_bGuessCorrect;
}

string  GameMessageGuessResult::getGuessCorrectAsString ()
{
    if(m_bGuessCorrect)
    {
        return "Yes";
    }
    else
    {
        return "No";
    }
}

/** Confirm that the JSON to be processed contains the right fields */
bool   GameMessageGuessResult::confirmValidMessage (Document * pDoc)
{
    map<string, string>     theMap;

    theMap.insert(pair<string, string>("Winner", "string"));
    theMap.insert(pair<string, string>("PlayerInfo", "array"));

    if(!confirmValidMessage_Data(pDoc, &theMap))
        return false;


    map<string, string>     theMapInfo;

// Name - Player name
//  Number - Player number
//  Correct - Yes or No if the entire guess was correct  
//  ReceiptTime - Time in UTC including seconds and microseconds
//  Result - String containing a set of B, Y, G which refer to black (B - wrong guess), yellow (Y - right letter, wrong spot), and green (G - right letter, right spot)

    theMapInfo.insert(pair<string, string>("Name", "string"));
    theMapInfo.insert(pair<string, string>("Number", "number"));
    theMapInfo.insert(pair<string, string>("Correct", "string"));
    theMapInfo.insert(pair<string, string>("ReceiptTime", "number"));
    theMapInfo.insert(pair<string, string>("Result", "string"));

    if(!confirmValidMessage_PlayerInfo(pDoc, &theMapInfo))
        return false;

    return true;
}


string GameMessageGuessResult::getJSON_Data ()
{
    string  sMessage;

    sMessage = "{ \"Winner\" : \"" + getGuessCorrectAsString() + "\", \"PlayerInfo\" : ["; 

    for(int j=0; j<getPlayerInfo()->size(); j++)
    {
        sMessage += "{";
        sMessage += "\"Name\" : \"" + getPlayerInfo()->at(j)->getName() + "\", ";
        sMessage += "\"Number\" : " + getPlayerInfo()->at(j)->getNumberAsString() + ", ";
        sMessage += "\"Correct\" : \"" + getPlayerInfo()->at(j)->getWinnerAsString() + "\", ";
        sMessage += "\"ReceiptTime\" : " + getPlayerInfo()->at(j)->getReceiptTimeAsString() + ", ";
        sMessage += "\"Result\" : \"" + getPlayerInfo()->at(j)->getResult() + "\" ";
        sMessage += "}";

        if(j+1 < getPlayerInfo()->size())
        {
            sMessage += ", ";
        }
    }

    sMessage += "] ";
    sMessage += "}";

    return sMessage;
}




///////////////////////////////////////////////////////////////

GameMessageEndRound::GameMessageEndRound ()
{
    setType(MESSAGETYPE_END_ROUND);
    m_nRoundsRemaining = 0;
}

GameMessageEndRound::~GameMessageEndRound ()
{
    clearInfo();
}

int GameMessageEndRound::getRoundsRemaining ()
{
    return m_nRoundsRemaining;
}

string GameMessageEndRound::getRoundsRemainingAsString ()
{
    char szBuffer[20];

    sprintf(szBuffer, "%d", m_nRoundsRemaining);

    return szBuffer;
}

void GameMessageEndRound::setRoundsRemaining (int nRounds)
{
    m_nRoundsRemaining = nRounds;
}

/** Confirm that the JSON to be processed contains the right fields */
bool   GameMessageEndRound::confirmValidMessage (Document * pDoc)
{
    map<string, string>     theMap;

    theMap.insert(pair<string, string>("RoundsRemaining", "number"));
    theMap.insert(pair<string, string>("PlayerInfo", "array"));

    if(!confirmValidMessage_Data(pDoc, &theMap))
        return false;


    map<string, string>     theMapInfo;

    theMapInfo.insert(pair<string, string>("Name", "string"));
    theMapInfo.insert(pair<string, string>("Number", "number"));
    theMapInfo.insert(pair<string, string>("ScoreEarned", "number"));
    theMapInfo.insert(pair<string, string>("Winner", "string"));

    if(!confirmValidMessage_PlayerInfo(pDoc, &theMapInfo))
        return false;

    return true;
}


string GameMessageEndRound::getJSON_Data ()
{
    string  sMessage;

    sMessage = "{ \"RoundsRemaining\" : " + getRoundsRemainingAsString() + ", \"PlayerInfo\" : ["; 

    for(int j=0; j<getPlayerInfo()->size(); j++)
    {
        sMessage += "{";
        sMessage += "\"Name\" : \"" + getPlayerInfo()->at(j)->getName() + "\", ";
        sMessage += "\"Number\" : " + getPlayerInfo()->at(j)->getNumberAsString() + ", ";
        sMessage += "\"ScoreEarned\" : " + getPlayerInfo()->at(j)->getScoreAsString() + ", ";
        sMessage += "\"Winner\" : \"" + getPlayerInfo()->at(j)->getWinnerAsString() + "\" ";
        sMessage += "}";

        if(j+1 < getPlayerInfo()->size())
        {
            sMessage += ", ";
        }
    }

    sMessage += "] ";
    sMessage += "}";

    return sMessage;
}

///////////////////////////////////////////////////////////////

GameMessageEndGame::GameMessageEndGame ()
{
    setType(MESSAGETYPE_END_GAME);
    m_sWinner = "";
}

GameMessageEndGame::~GameMessageEndGame ()
{
    clearInfo();
}

string GameMessageEndGame::getWinner ()
{
    return m_sWinner;
}

void GameMessageEndGame::setWinner (string sWinner)
{
    m_sWinner = sWinner;
}

string GameMessageEndGame::getJSON_Data ()
{
    string  sMessage;

    sMessage = "{ \"WinnerName\" : \"" + m_sWinner + "\", \"PlayerInfo\" : ["; 

    for(int j=0; j<getPlayerInfo()->size(); j++)
    {
        sMessage += "{";
        sMessage += "\"Name\" : \"" + getPlayerInfo()->at(j)->getName() + "\", ";
        sMessage += "\"Number\" : " + getPlayerInfo()->at(j)->getNumberAsString() + ", ";
        sMessage += "\"Score\" : " + getPlayerInfo()->at(j)->getScoreAsString() + " ";
        sMessage += "}";

        if(j+1 < getPlayerInfo()->size())
        {
            sMessage += ", ";
        }
    }

    sMessage += "] ";
    sMessage += "}";

    return sMessage;
}

/** Confirm that the JSON to be processed contains the right fields */
bool   GameMessageEndGame::confirmValidMessage (Document * pDoc)
{
    map<string, string>     theMap;

    theMap.insert(pair<string, string>("WinnerName", "string"));
    theMap.insert(pair<string, string>("PlayerInfo", "array"));

    if(!confirmValidMessage_Data(pDoc, &theMap))
        return false;


    map<string, string>     theMapInfo;

    theMapInfo.insert(pair<string, string>("Name", "string"));
    theMapInfo.insert(pair<string, string>("Number", "number"));
    theMapInfo.insert(pair<string, string>("Score", "number"));

    if(!confirmValidMessage_PlayerInfo(pDoc, &theMapInfo))
        return false;

    return true;
}







GameMessage * extractFromJSON (string theMessage)
{
    GameMessage *   pMessage;
    Document        theDocument;

    pMessage = NULL;

    if(theDocument.Parse(theMessage.c_str()).HasParseError())
    {
        cerr << "Error: Badly formed JSON by rapidjson" << endl;
        cerr << "    JSON was: " << theMessage << endl;
        return NULL;
    }

    if(!theDocument.IsObject())
    {
        cerr << "Error: The root of the JSON was not an object (dictionary)" << endl;
        cerr << "  IsObject returns false - curly braces were not used at the root" << endl;
        cerr << "  JSON was: " << theMessage << endl;
        return NULL;
    }

    if(!theDocument.HasMember("MessageType"))
    {
        cerr << "Error: The JSON does not contain a MessageType member (field)" << endl;
        cerr << "  JSON was: " << theMessage << endl;
        return NULL;        
    }

    if(!theDocument.HasMember("Data"))
    {
        cerr << "Error: The JSON does not contain a Data member (field)" << endl;
        cerr << "  JSON was: " << theMessage << endl;
        return NULL;        
    }

    if(!theDocument["MessageType"].IsString())
    {
        cerr << "Error: The JSON MessageType value is not a string" << endl;
        cerr << "  JSON was: " << theMessage << endl;
        return NULL;        
    }

    if(!theDocument["Data"].IsObject())
    {
        cerr << "Error: The JSON Data value is not an object (dictionary) - aka curly braces" << endl;
        cerr << "  JSON was: " << theMessage << endl;
        return NULL;        
    }

    if(theDocument["MessageType"] == "Join")
    {
        // Join message should have the following components
        //
        //  Name   - The name of the player to use in the game
        //  Client - An identifier for the particular version / client information

        pMessage = new GameMessageJoin();

        if(!theDocument["Data"].HasMember("Name"))
        {
            cerr << "Error: The Join message does not have a Name member" << endl;
            cerr << "  JSON was: " << theMessage << endl;
            delete pMessage;
            return NULL;
        }

        if(!theDocument["Data"].HasMember("Client"))
        {
            cerr << "Error: The Join message does not have a Client member" << endl;
            cerr << "  JSON was: " << theMessage << endl;
            delete pMessage;
            return NULL;
        }

        if(!theDocument["Data"]["Name"].IsString())
        {
            cerr << "Error: The Join message Name member is not a string" << endl;
            cerr << "  JSON was: " << theMessage << endl;
            delete pMessage;
            return NULL;
        }

        if(!theDocument["Data"]["Client"].IsString())
        {
            cerr << "Error: The Join message Client member is not a string" << endl;
            cerr << "  JSON was: " << theMessage << endl;
            delete pMessage;
            return NULL;
        }

        ((GameMessageJoin *) pMessage)->setName(theDocument["Data"]["Name"].GetString());
        ((GameMessageJoin *) pMessage)->setClientInfo(theDocument["Data"]["Client"].GetString());

        return pMessage;
    }
    else if(theDocument["MessageType"] == "JoinResult")
    {
        // JoinResult message should have the following components
        //
        //  Name   - The name of the player to use in the game
        //  Result - Yes or No for success or failure

        pMessage = new GameMessageJoinResult();

        if(!theDocument["Data"].HasMember("Name"))
        {
            cerr << "Error: The JoinResult message does not have a Name member" << endl;
            cerr << "  JSON was: " << theMessage << endl;
            delete pMessage;
            return NULL;
        }

        if(!theDocument["Data"].HasMember("Result"))
        {
            cerr << "Error: The JoinResult message does not have a Result member" << endl;
            cerr << "  JSON was: " << theMessage << endl;
            delete pMessage;
            return NULL;
        }

        if(!theDocument["Data"]["Name"].IsString())
        {
            cerr << "Error: The JoinResult message Name member is not a string" << endl;
            cerr << "  JSON was: " << theMessage << endl;
            delete pMessage;
            return NULL;
        }

        if(!theDocument["Data"]["Result"].IsString())
        {
            cerr << "Error: The JoinResult message Result member is not a string" << endl;
            cerr << "  JSON was: " << theMessage << endl;
            delete pMessage;
            return NULL;
        }

        ((GameMessageJoinResult *) pMessage)->setName(theDocument["Data"]["Name"].GetString());
        ((GameMessageJoinResult *) pMessage)->setResult(theDocument["Data"]["Result"].GetString());

        return pMessage;
    }
    else if(theDocument["MessageType"] == "Chat")
    {
        // Chat message should have the following components
        //
        //  Name - The name of the player doing the chat
        //  Chat - The chat message

        pMessage = new GameMessageChat();

        if(!theDocument["Data"].HasMember("Name"))
        {
            cerr << "Error: The Join message does not have a Name member" << endl;
            cerr << "  JSON was: " << theMessage << endl;
            delete pMessage;
            return NULL;
        }

        if(!theDocument["Data"].HasMember("Chat"))
        {
            cerr << "Error: The Join message does not have a Chat member" << endl;
            cerr << "  JSON was: " << theMessage << endl;
            delete pMessage;
            return NULL;
        }

        if(!theDocument["Data"]["Name"].IsString())
        {
            cerr << "Error: The Join message Name member is not a string" << endl;
            cerr << "  JSON was: " << theMessage << endl;
            delete pMessage;
            return NULL;
        }

        if(!theDocument["Data"]["Chat"].IsString())
        {
            cerr << "Error: The Join message Chat member is not a string" << endl;
            cerr << "  JSON was: " << theMessage << endl;
            delete pMessage;
            return NULL;
        }

        ((GameMessageChat *) pMessage)->setName(theDocument["Data"]["Name"].GetString());
        ((GameMessageChat *) pMessage)->setChat(theDocument["Data"]["Chat"].GetString());

        return pMessage;
    }
    else if(theDocument["MessageType"] == "StartInstance")
    {
        // StartInstance message should have the following components
        //
        //  Server  - The server hostname or IP address
        //  Port  - The port number
        //  Nonce - The nonce that the client should use 

        pMessage = new GameMessageStartInstance();

        if(!theDocument["Data"].HasMember("Server"))
        {
            cerr << "Error: The StartInstance message does not have a Server member" << endl;
            cerr << "  JSON was: " << theMessage << endl;
            delete pMessage;
            return NULL;
        }

        if(!theDocument["Data"].HasMember("Port"))
        {
            cerr << "Error: The StartInstance message does not have a Port member" << endl;
            cerr << "  JSON was: " << theMessage << endl;
            delete pMessage;
            return NULL;
        }

        if(!theDocument["Data"].HasMember("Nonce"))
        {
            cerr << "Error: The StartInstance message does not have a Nonce member" << endl;
            cerr << "  JSON was: " << theMessage << endl;
            delete pMessage;
            return NULL;
        }

        if(!theDocument["Data"]["Server"].IsString())
        {
            cerr << "Error: The StartInstance message Server member is not a string" << endl;
            cerr << "  JSON was: " << theMessage << endl;
            delete pMessage;
            return NULL;
        }

        if(!theDocument["Data"]["Port"].IsString() && !theDocument["Data"]["Port"].IsNumber())
        {
            cerr << "Error: The StartInstance message Port member is not a string" << endl;
            cerr << "  JSON was: " << theMessage << endl;
            delete pMessage;
            return NULL;
        }

        if(!theDocument["Data"]["Nonce"].IsString() && !theDocument["Data"]["Nonce"].IsNumber())
        {
            cerr << "Error: The StartInstance message Nonce member is not a string" << endl;
            cerr << "  JSON was: " << theMessage << endl;
            delete pMessage;
            return NULL;
        }

        ((GameMessageStartInstance *) pMessage)->setServerName(theDocument["Data"]["Server"].GetString());

        if(theDocument["Data"]["Port"].IsNumber())
        {
            ((GameMessageStartInstance *) pMessage)->setPort(theDocument["Data"]["Port"].GetInt());
        }
        else 
        {
            ((GameMessageStartInstance *) pMessage)->setPort(theDocument["Data"]["Port"].GetString());
        }

        if(theDocument["Data"]["Nonce"].IsNumber())
        {
            ((GameMessageStartInstance *) pMessage)->setNonce(theDocument["Data"]["Nonce"].GetInt());
        }
        else 
        {
            ((GameMessageStartInstance *) pMessage)->setNonce(theDocument["Data"]["Nonce"].GetString());
        }

        return pMessage;


    }
    else if(theDocument["MessageType"] == "JoinInstance")
    {
        // JoinInstance message should have the following components
        //
        //  Name  - The name of the player doing the chat
        //  Nonce - The nonce unique to the client

        pMessage = new GameMessageJoinInstance();

        if(!theDocument["Data"].HasMember("Name"))
        {
            cerr << "Error: The JoinInstance message does not have a Name member" << endl;
            cerr << "  JSON was: " << theMessage << endl;
            delete pMessage;
            return NULL;
        }

        if(!theDocument["Data"].HasMember("Nonce"))
        {
            cerr << "Error: The JoinInstance message does not have a Nonce member" << endl;
            cerr << "  JSON was: " << theMessage << endl;
            delete pMessage;
            return NULL;
        }

        if(!theDocument["Data"]["Name"].IsString())
        {
            cerr << "Error: The JoinInstance message Name member is not a string" << endl;
            cerr << "  JSON was: " << theMessage << endl;
            delete pMessage;
            return NULL;
        }

        if(!theDocument["Data"]["Nonce"].IsString() && !theDocument["Data"]["Nonce"].IsNumber())
        {
            cerr << "Error: The JoinInstance message Nonce member is not a string or a number" << endl;
            cerr << "  JSON was: " << theMessage << endl;
            delete pMessage;
            return NULL;
        }

        ((GameMessageJoinInstance *) pMessage)->setName(theDocument["Data"]["Name"].GetString());

        if(theDocument["Data"]["Nonce"].IsNumber())
        {
            ((GameMessageJoinInstance *) pMessage)->setNonce(theDocument["Data"]["Nonce"].GetInt());
        }
        else 
        {
            ((GameMessageJoinInstance *) pMessage)->setNonce(theDocument["Data"]["Nonce"].GetString());
        }

        return pMessage;
    }
    else if(theDocument["MessageType"] == "JoinInstanceResult")
    {
        // JoinInstanceResult message should have the following components
        //
        //  Name   - The name of the player doing the chat
        //  Number - The number for the player (integer)
        //  Result - The result of the join (yes or no)

        pMessage = new GameMessageJoinInstanceResult();

        if(!theDocument["Data"].HasMember("Name"))
        {
            cerr << "Error: The JoinInstanceResult message does not have a Name member" << endl;
            cerr << "  JSON was: " << theMessage << endl;
            delete pMessage;
            return NULL;
        }

        if(!theDocument["Data"].HasMember("Number"))
        {
            cerr << "Error: The JoinInstanceResult message does not have a Nonce member" << endl;
            cerr << "  JSON was: " << theMessage << endl;
            delete pMessage;
            return NULL;
        }

        if(!theDocument["Data"].HasMember("Result"))
        {
            cerr << "Error: The JoinInstanceResult message does not have a Result member" << endl;
            cerr << "  JSON was: " << theMessage << endl;
            delete pMessage;
            return NULL;
        }


        if(!theDocument["Data"]["Name"].IsString())
        {
            cerr << "Error: The JoinInstanceResult message Name member is not a string" << endl;
            cerr << "  JSON was: " << theMessage << endl;
            delete pMessage;
            return NULL;
        }

        if(!theDocument["Data"]["Number"].IsNumber())
        {
            cerr << "Error: The JoinInstanceResult message Number member is not a number" << endl;
            cerr << "  JSON was: " << theMessage << endl;
            delete pMessage;
            return NULL;
        }


        if(!theDocument["Data"]["Result"].IsString())
        {
            cerr << "Error: The JoinInstanceResult message Result member is not a string" << endl;
            cerr << "  JSON was: " << theMessage << endl;
            delete pMessage;
            return NULL;
        }

        ((GameMessageJoinInstanceResult *) pMessage)->setName(theDocument["Data"]["Name"].GetString());
        ((GameMessageJoinInstanceResult *) pMessage)->setNumber(theDocument["Data"]["Number"].GetInt());
        ((GameMessageJoinInstanceResult *) pMessage)->setResult(theDocument["Data"]["Result"].GetString());

        return pMessage;
    }
    else if(theDocument["MessageType"] == "StartGame")
    {
        // StartGame message should have the following components
        //
        //  Rounds     - The number of rounds in the game
        //  PlayerInfo - An array of information about the players containing:
        //     Name   - Name of the player
        //     Number - Position of the player

        pMessage = new GameMessageStartGame();

        if(!theDocument["Data"].HasMember("Rounds"))
        {
            cerr << "Error: The StartGame message does not have a Rounds member" << endl;
            cerr << "  JSON was: " << theMessage << endl;
            delete pMessage;
            return NULL;
        }

        if(!theDocument["Data"].HasMember("PlayerInfo"))
        {
            cerr << "Error: The StartGame message does not have a PlayerInfo member" << endl;
            cerr << "  JSON was: " << theMessage << endl;
            delete pMessage;
            return NULL;
        }

        if(!theDocument["Data"]["Rounds"].IsNumber())
        {
            cerr << "Error: The StartGame message Rounds member is not a number" << endl;
            cerr << "  JSON was: " << theMessage << endl;
            delete pMessage;
            return NULL;
        }

        if(!theDocument["Data"]["PlayerInfo"].IsArray())
        {
            cerr << "Error: The StartGame message PlayerInfo member is not an array" << endl;
            cerr << "  JSON was: " << theMessage << endl;
            delete pMessage;
            return NULL;
        }

        // Now we need a bit of trickery looping through the array inside of PlayerInfo

        for(int j=0; j<theDocument["Data"]["PlayerInfo"].Size(); j++)
        {
            if(!theDocument["Data"]["PlayerInfo"][j].HasMember("Name"))            
            {
                cerr << "Error: The StartGame message PlayerInfo array element " << j << " does not have a Name member" << endl;
                cerr << "  JSON was: " << theMessage << endl;                
                delete pMessage;
                return NULL;
            }

            if(!theDocument["Data"]["PlayerInfo"][j].HasMember("Number"))            
            {
                cerr << "Error: The StartGame message PlayerInfo array element " << j << " does not have a Number member" << endl;
                cerr << "  JSON was: " << theMessage << endl;
                delete pMessage;
                return NULL;
            }

            if(!theDocument["Data"]["PlayerInfo"][j]["Name"].IsString())            
            {
                cerr << "Error: The StartGame message PlayerInfo array element " << j << " has a Name member that is not a string" << endl;
                cerr << "  JSON was: " << theMessage << endl;
                delete pMessage;
                return NULL;
            }

            if(!theDocument["Data"]["PlayerInfo"][j]["Number"].IsNumber())            
            {
                cerr << "Error: The StartGame message PlayerInfo array element " << j << " has a Number member that is not a number" << endl;
                cerr << "  JSON was: " << theMessage << endl;
                delete pMessage;
                return NULL;
            }

            PlayerInfo * pInfo;

            pInfo = new PlayerInfo();
            pInfo->setName(theDocument["Data"]["PlayerInfo"][j]["Name"].GetString());
            pInfo->setNumber(theDocument["Data"]["PlayerInfo"][j]["Number"].GetInt());

            ((GameMessageStartGame *)pMessage)->addInfo(pInfo);
        }

        ((GameMessageStartGame *)pMessage)->setRounds(theDocument["Data"]["Rounds"].GetInt());
        return pMessage;
    }
    else if(theDocument["MessageType"] == "StartRound")
    {
        // StartRound message should have the following components
        //
        //  Rounds          - The number of rounds in the game
        //  RoundsRemaining - The number of rounds remaining in the game
        //  Length          - The length of the word for this round

        //  PlayerInfo - An array of information about the players containing:
        //     Name   - Name of the player
        //     Number - Position of the player
        //     Score  - Score of the player          

        pMessage = new GameMessageStartRound();

        if(!theDocument["Data"].HasMember("Rounds"))
        {
            cerr << "Error: The StartRound message does not have a Rounds member" << endl;
            cerr << "  JSON was: " << theMessage << endl;
            delete pMessage;
            return NULL;
        }

        if(!theDocument["Data"].HasMember("RoundsRemaining"))
        {
            cerr << "Error: The StartRound message does not have a RoundsRemaining member" << endl;
            cerr << "  JSON was: " << theMessage << endl;
            delete pMessage;
            return NULL;
        }

        if(!theDocument["Data"].HasMember("Length"))
        {
            cerr << "Error: The StartRound message does not have a Length member" << endl;
            cerr << "  JSON was: " << theMessage << endl;
            delete pMessage;
            return NULL;
        }

        if(!theDocument["Data"].HasMember("PlayerInfo"))
        {
            cerr << "Error: The StartRound message does not have a PlayerInfo member" << endl;
            cerr << "  JSON was: " << theMessage << endl;
            delete pMessage;
            return NULL;
        }

        if(!theDocument["Data"]["Rounds"].IsNumber())
        {
            cerr << "Error: The StartRound message Rounds member is not a number" << endl;
            cerr << "  JSON was: " << theMessage << endl;
            delete pMessage;
            return NULL;
        }

        if(!theDocument["Data"]["RoundsRemaining"].IsNumber())
        {
            cerr << "Error: The StartRound message RoundsRemaining member is not a number" << endl;
            cerr << "  JSON was: " << theMessage << endl;
            delete pMessage;
            return NULL;
        }

        if(!theDocument["Data"]["Length"].IsNumber())
        {
            cerr << "Error: The StartRound message Length member is not a number" << endl;
            cerr << "  JSON was: " << theMessage << endl;
            delete pMessage;
            return NULL;
        }

        if(!theDocument["Data"]["PlayerInfo"].IsArray())
        {
            cerr << "Error: The StartGame message PlayerInfo member is not an array" << endl;
            cerr << "  JSON was: " << theMessage << endl;
            delete pMessage;
            return NULL;
        }

        // Now we need a bit of trickery looping through the array inside of PlayerInfo

        for(int j=0; j<theDocument["Data"]["PlayerInfo"].Size(); j++)
        {
            if(!theDocument["Data"]["PlayerInfo"][j].HasMember("Name"))            
            {
                cerr << "Error: The StartGame message PlayerInfo array element " << j << " does not have a Name member" << endl;
                cerr << "  JSON was: " << theMessage << endl;                
                delete pMessage;
                return NULL;
            }

            if(!theDocument["Data"]["PlayerInfo"][j].HasMember("Number"))            
            {
                cerr << "Error: The StartGame message PlayerInfo array element " << j << " does not have a Number member" << endl;
                cerr << "  JSON was: " << theMessage << endl;
                delete pMessage;
                return NULL;
            }

            if(!theDocument["Data"]["PlayerInfo"][j].HasMember("Score"))            
            {
                cerr << "Error: The StartGame message PlayerInfo array element " << j << " does not have a Score member" << endl;
                cerr << "  JSON was: " << theMessage << endl;
                delete pMessage;
                return NULL;
            }

            if(!theDocument["Data"]["PlayerInfo"][j]["Name"].IsString())            
            {
                cerr << "Error: The StartGame message PlayerInfo array element " << j << " has a Name member that is not a string" << endl;
                cerr << "  JSON was: " << theMessage << endl;
                delete pMessage;
                return NULL;
            }

            if(!theDocument["Data"]["PlayerInfo"][j]["Number"].IsNumber())            
            {
                cerr << "Error: The StartGame message PlayerInfo array element " << j << " has a Number member that is not a number" << endl;
                cerr << "  JSON was: " << theMessage << endl;
                delete pMessage;
                return NULL;
            }

            if(!theDocument["Data"]["PlayerInfo"][j]["Score"].IsNumber())            
            {
                cerr << "Error: The StartGame message PlayerInfo array element " << j << " has a Score member that is not a number" << endl;
                cerr << "  JSON was: " << theMessage << endl;
                delete pMessage;
                return NULL;
            }

            PlayerInfo * pInfo;

            pInfo = new PlayerInfo();
            pInfo->setName(theDocument["Data"]["PlayerInfo"][j]["Name"].GetString());
            pInfo->setNumber(theDocument["Data"]["PlayerInfo"][j]["Number"].GetInt());
            pInfo->setScore(theDocument["Data"]["PlayerInfo"][j]["Score"].GetInt());

            ((GameMessageStartRound *)pMessage)->addInfo(pInfo);
        }

        ((GameMessageStartRound *)pMessage)->setRounds(theDocument["Data"]["Rounds"].GetInt());
        ((GameMessageStartRound *)pMessage)->setRoundsRemaining(theDocument["Data"]["RoundsRemaining"].GetInt());
        ((GameMessageStartRound *)pMessage)->setLength(theDocument["Data"]["Length"].GetInt());

        return pMessage;        
    }
    else if(theDocument["MessageType"] == "PromptForGuess")
    {
        pMessage = new GameMessagePromptForGuess();

        if(!pMessage->confirmValidMessage (&theDocument))
        {
            delete pMessage;
            return NULL;
        }   

        ((GameMessagePromptForGuess *)pMessage)->setWordLength(theDocument["Data"]["WordLength"].GetInt());     
        ((GameMessagePromptForGuess *)pMessage)->setName(theDocument["Data"]["Name"].GetString());     
        ((GameMessagePromptForGuess *)pMessage)->setGuessNumber(theDocument["Data"]["GuessNumber"].GetInt());     
        
        return pMessage;        
    }
    else if(theDocument["MessageType"] == "Guess")
    {
        // Guess message should have the following components
        //
        //  Name   - The name of the player to use in the game
        //  Guess  - The guess itself

        pMessage = new GameMessageGuess();

        if(!theDocument["Data"].HasMember("Name"))
        {
            cerr << "Error: The Guess message does not have a Name member" << endl;
            cerr << "  JSON was: " << theMessage << endl;
            delete pMessage;
            return NULL;
        }

        if(!theDocument["Data"].HasMember("Guess"))
        {
            cerr << "Error: The Guess message does not have a Guess member" << endl;
            cerr << "  JSON was: " << theMessage << endl;
            delete pMessage;
            return NULL;
        }

        if(!theDocument["Data"]["Name"].IsString())
        {
            cerr << "Error: The Guess message Name member is not a string" << endl;
            cerr << "  JSON was: " << theMessage << endl;
            delete pMessage;
            return NULL;
        }

        if(!theDocument["Data"]["Guess"].IsString())
        {
            cerr << "Error: The Guess message Guess member is not a string" << endl;
            cerr << "  JSON was: " << theMessage << endl;
            delete pMessage;
            return NULL;
        }

        ((GameMessageGuess *) pMessage)->setName(theDocument["Data"]["Name"].GetString());
        ((GameMessageGuess *) pMessage)->setGuess(theDocument["Data"]["Guess"].GetString());

        return pMessage;        
    }
    else if(theDocument["MessageType"] == "GuessResponse")
    {
        // GuessResponse message should have the following components
        //
        //  Name   - The name of the player to use in the game
        //  Guess  - The guess itself
        //  Accepted - Yes or No if the guess is accepted

        pMessage = new GameMessageGuessResponse();

        if(!theDocument["Data"].HasMember("Name"))
        {
            cerr << "Error: The GuessResponse message does not have a Name member" << endl;
            cerr << "  JSON was: " << theMessage << endl;
            delete pMessage;
            return NULL;
        }

        if(!theDocument["Data"].HasMember("Guess"))
        {
            cerr << "Error: The GuessResponse message does not have a Guess member" << endl;
            cerr << "  JSON was: " << theMessage << endl;
            delete pMessage;
            return NULL;
        }

        if(!theDocument["Data"].HasMember("Accepted"))
        {
            cerr << "Error: The GuessResponse message does not have an Accepted member" << endl;
            cerr << "  JSON was: " << theMessage << endl;
            delete pMessage;
            return NULL;
        }


        if(!theDocument["Data"]["Name"].IsString())
        {
            cerr << "Error: The GuessResponse message Name member is not a string" << endl;
            cerr << "  JSON was: " << theMessage << endl;
            delete pMessage;
            return NULL;
        }

        if(!theDocument["Data"]["Guess"].IsString())
        {
            cerr << "Error: The GuessResponse message Guess member is not a string" << endl;
            cerr << "  JSON was: " << theMessage << endl;
            delete pMessage;
            return NULL;
        }

        if(!theDocument["Data"]["Accepted"].IsString())
        {
            cerr << "Error: The GuessResponse message Accepted member is not a string" << endl;
            cerr << "  JSON was: " << theMessage << endl;
            delete pMessage;
            return NULL;
        }


        ((GameMessageGuessResponse *) pMessage)->setName(theDocument["Data"]["Name"].GetString());
        ((GameMessageGuessResponse *) pMessage)->setGuess(theDocument["Data"]["Guess"].GetString());
        ((GameMessageGuessResponse *) pMessage)->setAccepted(theDocument["Data"]["Accepted"].GetString());

        return pMessage;        
    }
    else if(theDocument["MessageType"] == "GuessStatus")
    {
        pMessage = new GameMessageGuessStatus();

        if(!pMessage->confirmValidMessage (&theDocument))
        {
            delete pMessage;
            return NULL;
        }        
        
        return pMessage;
    }
    else if(theDocument["MessageType"] == "GuessResult")
    {
        pMessage = new GameMessageGuessResult();

        if(!pMessage->confirmValidMessage (&theDocument))
        {
            delete pMessage;
            return NULL;
        }

        // If it passed the validation step, we just need to parse it as we know all of the 
        //  fields that we need are present. We will effectively ignore any extraneous
        //  fields.

        for(int j=0; j<theDocument["Data"]["PlayerInfo"].Size(); j++)
        {
            PlayerInfo * pInfo;

            pInfo = new PlayerInfo();
            pInfo->setName(theDocument["Data"]["PlayerInfo"][j]["Name"].GetString());
            pInfo->setNumber(theDocument["Data"]["PlayerInfo"][j]["Number"].GetInt());
            pInfo->setWinner(theDocument["Data"]["PlayerInfo"][j]["Correct"].GetString());
            pInfo->setReceiptTime(theDocument["Data"]["PlayerInfo"][j]["ReceiptTime"].GetDouble());
            pInfo->setResult(theDocument["Data"]["PlayerInfo"][j]["Result"].GetString());

            ((GameMessageGuessResult *)pMessage)->addInfo(pInfo);
        }

        ((GameMessageGuessResult *)pMessage)->setGuessCorrect(theDocument["Data"]["Winner"].GetString());
        return pMessage;         

    }
    else if(theDocument["MessageType"] == "EndRound")
    {
        pMessage = new GameMessageEndRound();

        if(!pMessage->confirmValidMessage (&theDocument))
        {
            delete pMessage;
            return NULL;
        }

        // If it passed the validation step, we just need to parse it as we know all of the 
        //  fields that we need are present. We will effectively ignore any extraneous
        //  fields.

        for(int j=0; j<theDocument["Data"]["PlayerInfo"].Size(); j++)
        {
            PlayerInfo * pInfo;

            pInfo = new PlayerInfo();
            pInfo->setName(theDocument["Data"]["PlayerInfo"][j]["Name"].GetString());
            pInfo->setNumber(theDocument["Data"]["PlayerInfo"][j]["Number"].GetInt());
            pInfo->setScore(theDocument["Data"]["PlayerInfo"][j]["ScoreEarned"].GetInt());
            pInfo->setWinner(theDocument["Data"]["PlayerInfo"][j]["Winner"].GetString());

            ((GameMessageEndRound *)pMessage)->addInfo(pInfo);
        }

        ((GameMessageEndRound *)pMessage)->setRoundsRemaining(theDocument["Data"]["RoundsRemaining"].GetInt());
        return pMessage;         
    }
    else if(theDocument["MessageType"] == "EndGame")
    {
        pMessage = new GameMessageEndGame();

        if(!pMessage->confirmValidMessage (&theDocument))
        {
            delete pMessage;
            return NULL;
        }

        // If it passed the validation step, we just need to parse it as we know all of the 
        //  fields that we need are present. We will effectively ignore any extraneous
        //  fields.

        for(int j=0; j<theDocument["Data"]["PlayerInfo"].Size(); j++)
        {
            PlayerInfo * pInfo;

            pInfo = new PlayerInfo();
            pInfo->setName(theDocument["Data"]["PlayerInfo"][j]["Name"].GetString());
            pInfo->setNumber(theDocument["Data"]["PlayerInfo"][j]["Number"].GetInt());
            pInfo->setScore(theDocument["Data"]["PlayerInfo"][j]["Score"].GetInt());

            ((GameMessageEndGame *)pMessage)->addInfo(pInfo);
        }

        ((GameMessageEndGame *)pMessage)->setWinner(theDocument["Data"]["WinnerName"].GetString());

        return pMessage;         
    }
    else
    {
        cerr << "Error: Unexpected JSON message of MessageType " << theDocument["MessageType"].GetString() << endl;
        cerr << "  Ignoring the message" << endl;
        cerr << "  JSON was: " << theMessage << endl;

        delete pMessage;
        return NULL;
    }



    return pMessage;
}


void testsuite_Message ()
{
    GameMessage * pMessage;

    const char json_join[] = " { \"MessageType\" : \"Join\",  \"Data\" : { \"Name\" : \"striegel\", \"Client\" : \"TestOnly\"  } } ";

    pMessage = extractFromJSON(string(json_join));

    if(pMessage != NULL)
    {
        delete pMessage;
    }
    else 
    {
        cerr << "Error: Failed to extract pre-defined Join message" << endl;
    }

    // Test the join message

    GameMessageJoin     theJoin;

    theJoin.setName("striegel");
    theJoin.setClientInfo("TheMatrix");

    queueOutput("== Join Message Test ==\n");
    queueOutput("  ");
    queueOutput(theJoin.getJSON());
    queueOutput("\n");

    pMessage = extractFromJSON(theJoin.getJSON());

    if(pMessage != NULL && pMessage->getType() == MESSAGETYPE_JOIN)
    {
        queueOutput(" Join Message .... passed\n");
    }
    else
    {
        queueOutput(" Join Message .... failed\n");
    }

    if(pMessage != NULL)
    {
        delete pMessage;
    }

    // Test the JoinResult message

    GameMessageJoinResult     theJoinResult;

    theJoinResult.setName("striegel");
    theJoinResult.setResult("No");

    queueOutput("== JoinResult Message Test ==\n");
    queueOutput("  ");
    queueOutput(theJoinResult.getJSON());
    queueOutput("\n");

    pMessage = extractFromJSON(theJoinResult.getJSON());

    if(pMessage != NULL && pMessage->getType() == MESSAGETYPE_JOIN_RESULT)
    {
        queueOutput(" JoinResult Message .... passed\n");
    }
    else
    {
        queueOutput(" JoinResult Message .... failed\n");
    }

    if(pMessage != NULL)
    {
        delete pMessage;
    }

    // Test the StartInstance message

    GameMessageStartInstance     theStartInstance;

    theStartInstance.setServerName("127.0.0.1");
    theStartInstance.setPort("42500");
    theStartInstance.setNonce("23827");

    queueOutput("== StartInstance Message Test ==\n");
    queueOutput("  ");
    queueOutput(theStartInstance.getJSON());
    queueOutput("\n");

    pMessage = extractFromJSON(theStartInstance.getJSON());

    if(pMessage != NULL && pMessage->getType() == MESSAGETYPE_START_INSTANCE)
    {
        queueOutput(" StartInstance Message .... passed\n");
    }
    else
    {
        queueOutput(" StartInstance Message .... failed\n");
    }

    if(pMessage != NULL)
    {
        delete pMessage;
    }

    // Test the JoinInstance message

    GameMessageJoinInstance     theJoinInstance;

    theJoinInstance.setName("striegel");
    theJoinInstance.setNonce("23827");

    queueOutput("== JoinInstance Message Test ==\n");
    queueOutput("  ");
    queueOutput(theJoinInstance.getJSON());
    queueOutput("\n");

    pMessage = extractFromJSON(theJoinInstance.getJSON());

    if(pMessage != NULL && pMessage->getType() == MESSAGETYPE_JOIN_INSTANCE)
    {
        queueOutput(" JoinInstance Message .... passed\n");
    }
    else
    {
        queueOutput(" JoinInstance Message .... failed\n");
    }

    if(pMessage != NULL)
    {
        delete pMessage;
    }

    // Test the JoinInstanceResult message

    GameMessageJoinInstanceResult     theJoinInstanceResult;

    theJoinInstanceResult.setName("striegel");
    theJoinInstanceResult.setNumber(1);
    theJoinInstanceResult.setResult("Yes");

    queueOutput("== JoinInstanceResult Message Test ==\n");
    queueOutput("  ");
    queueOutput(theJoinInstanceResult.getJSON());
    queueOutput("\n");

    pMessage = extractFromJSON(theJoinInstanceResult.getJSON());

    if(pMessage != NULL && pMessage->getType() == MESSAGETYPE_JOIN_INSTANCE_RESULT)
    {
        queueOutput(" JoinInstanceResult Message .... passed\n");
    }
    else
    {
        queueOutput(" JoinInstanceResult Message .... failed\n");
    }

    if(pMessage != NULL)
    {
        delete pMessage;
    }

    // Test the StartGame message

    GameMessageStartGame     theStartGame;

    theStartGame.setRounds(3);

    PlayerInfo *    pInfo;

    pInfo = new PlayerInfo ();
    pInfo->setName("striegel");
    pInfo->setNumber(1);
    pInfo->setScore(0);
    theStartGame.addInfo(pInfo);

    pInfo = new PlayerInfo ();
    pInfo->setName("not-striegel");
    pInfo->setNumber(2);
    pInfo->setScore(0);
    theStartGame.addInfo(pInfo);

    queueOutput("== StartGame Message Test ==\n");
    queueOutput("  ");
    queueOutput(theStartGame.getJSON());
    queueOutput("\n");

    pMessage = extractFromJSON(theStartGame.getJSON());

    if(pMessage != NULL && pMessage->getType() == MESSAGETYPE_START_GAME)
    {
        queueOutput(" StartGame Message .... passed\n");
    }
    else
    {
        queueOutput(" StartGame Message .... failed\n");
    }

    if(pMessage != NULL)
    {
        delete pMessage;
    }

    theStartGame.clearInfo();

    // Test the StartRound message

    GameMessageStartRound     theStartRound;

    theStartRound.setRounds(3);
    theStartRound.setRoundsRemaining(3);
    theStartRound.setLength(5);

//    PlayerInfo *    pInfo;

    pInfo = new PlayerInfo ();
    pInfo->setName("striegel");
    pInfo->setNumber(1);
    pInfo->setScore(0);
    theStartRound.addInfo(pInfo);

    pInfo = new PlayerInfo ();
    pInfo->setName("not-striegel");
    pInfo->setNumber(2);
    pInfo->setScore(0);
    theStartRound.addInfo(pInfo);

    queueOutput("== StartRound Message Test ==\n");
    queueOutput("  ");
    queueOutput(theStartRound.getJSON());
    queueOutput("\n");

    pMessage = extractFromJSON(theStartRound.getJSON());

    if(pMessage != NULL && pMessage->getType() == MESSAGETYPE_START_ROUND)
    {
        queueOutput(" StartRound Message .... passed\n");
    }
    else
    {
        queueOutput(" StartRound Message .... failed\n");
    }

    if(pMessage != NULL)
    {
        delete pMessage;
    }

    theStartRound.clearInfo();

    // Test the PromptForGuess message

    GameMessagePromptForGuess     theGuessPrompt;

    theGuessPrompt.setName("striegel");
    theGuessPrompt.setGuessNumber(1);
    theGuessPrompt.setWordLength(5);

    queueOutput("== PromptForGuess Message Test ==\n");
    queueOutput("  ");
    queueOutput(theGuessPrompt.getJSON());
    queueOutput("\n");

    pMessage = extractFromJSON(theGuessPrompt.getJSON());

    if(pMessage != NULL && pMessage->getType() == MESSAGETYPE_PROMPT_FOR_GUESS)
    {
        queueOutput(" PromptForGuess Message .... passed\n");
    }
    else
    {
        queueOutput(" PromptForGuess Message .... failed\n");
    }

    if(pMessage != NULL)
    {
        delete pMessage;
    }

    // Test the Guess message

    GameMessageGuess     theGuess;

    theGuess.setName("striegel");
    theGuess.setGuess("MPTCP");

    queueOutput("== Guess Message Test ==\n");
    queueOutput("  ");
    queueOutput(theGuess.getJSON());
    queueOutput("\n");

    pMessage = extractFromJSON(theGuess.getJSON());

    if(pMessage != NULL && pMessage->getType() == MESSAGETYPE_GUESS)
    {
        queueOutput(" Guess Message .... passed\n");
    }
    else
    {
        queueOutput(" Guess Message .... failed\n");
    }

    if(pMessage != NULL)
    {
        delete pMessage;
    }

    // Test the GuessResponse message

    GameMessageGuessResponse     theGuessResponse;

    theGuessResponse.setName("striegel");
    theGuessResponse.setGuess("MPTCP");
    theGuessResponse.setAccepted("Yes");

    queueOutput("== GuessResponse Message Test ==\n");
    queueOutput("  ");
    queueOutput(theGuessResponse.getJSON());
    queueOutput("\n");

    pMessage = extractFromJSON(theGuessResponse.getJSON());

    if(pMessage != NULL && pMessage->getType() == MESSAGETYPE_GUESS_RESPONSE)
    {
        queueOutput(" GuessResponse Message .... passed\n");
    }
    else
    {
        queueOutput(" GuessResponse Message .... failed\n");
    }

    if(pMessage != NULL)
    {
        delete pMessage;
    }

    // Test the Guess message

    GameMessageGuessStatus     theGuessStatus;

    queueOutput("== GuessStatus Message Test ==\n");
    queueOutput("  ");
    queueOutput(theGuessStatus.getJSON());
    queueOutput("\n");

    pMessage = extractFromJSON(theGuessStatus.getJSON());

    if(pMessage != NULL && pMessage->getType() == MESSAGETYPE_GUESS_STATUS)
    {
        queueOutput(" GuessStatus Message .... passed\n");
    }
    else
    {
        queueOutput(" GuessStatus Message .... failed\n");
    }

    if(pMessage != NULL)
    {
        delete pMessage;
    }

    // Test the GuessResult message

    GameMessageGuessResult     theGuessResult;
    timeval     theTime;

    theGuessResult.setGuessCorrect(true);

    pInfo = new PlayerInfo ();
    pInfo->setName("human");
    pInfo->setNumber(1);
    pInfo->setResult("GYYRG");
    pInfo->setWinner(false);
    gettimeofday(&theTime, NULL);
    pInfo->setReceiptTime(theTime);
    theGuessResult.addInfo(pInfo);

    pInfo = new PlayerInfo ();
    pInfo->setName("computer");
    pInfo->setNumber(2);
    pInfo->setResult("GGGGG");
    pInfo->setWinner(false);
    pInfo->setReceiptTime(42.42);
    theGuessResult.addInfo(pInfo);

    queueOutput("== GuessResult Message Test ==\n");
    queueOutput("  ");
    queueOutput(theGuessResult.getJSON());
    queueOutput("\n");

    pMessage = extractFromJSON(theGuessResult.getJSON());

    if(pMessage != NULL && pMessage->getType() == MESSAGETYPE_GUESS_RESULT)
    {
        queueOutput(" GuessResult Message .... passed\n");
    }
    else
    {
        queueOutput(" GuessResult Message .... failed\n");
    }

    if(pMessage != NULL)
    {
        delete pMessage;
    }

    theGuessResult.clearInfo();



    // Test the EndRound message

    GameMessageEndRound     theEndRound;

    theEndRound.setRoundsRemaining(0);

    pInfo = new PlayerInfo ();
    pInfo->setName("Strange");
    pInfo->setNumber(1);
    pInfo->setScore(25);
    pInfo->setWinner(true);
    theEndRound.addInfo(pInfo);

    pInfo = new PlayerInfo ();
    pInfo->setName("Stark");
    pInfo->setNumber(2);
    pInfo->setScore(25);
    pInfo->setWinner(true);
    theEndRound.addInfo(pInfo);

    queueOutput("== EndRound Message Test ==\n");
    queueOutput("  ");
    queueOutput(theEndRound.getJSON());
    queueOutput("\n");

    pMessage = extractFromJSON(theEndRound.getJSON());

    if(pMessage != NULL && pMessage->getType() == MESSAGETYPE_END_ROUND)
    {
        queueOutput(" EndRound Message .... passed\n");
    }
    else
    {
        queueOutput(" EndRound Message .... failed\n");
    }

    if(pMessage != NULL)
    {
        delete pMessage;
    }

    theEndRound.clearInfo();


    // Test the EndGame message

    GameMessageEndGame     theEndGame;

    theEndGame.setWinner("None");

    pInfo = new PlayerInfo ();
    pInfo->setName("striegel");
    pInfo->setNumber(1);
    pInfo->setScore(15);
    theEndGame.addInfo(pInfo);

    pInfo = new PlayerInfo ();
    pInfo->setName("not-striegel");
    pInfo->setNumber(2);
    pInfo->setScore(25);
    theEndGame.addInfo(pInfo);

    queueOutput("== EndGame Message Test ==\n");
    queueOutput("  ");
    queueOutput(theEndGame.getJSON());
    queueOutput("\n");

    pMessage = extractFromJSON(theEndGame.getJSON());

    if(pMessage != NULL && pMessage->getType() == MESSAGETYPE_END_GAME)
    {
        queueOutput(" EndGame Message .... passed\n");
    }
    else
    {
        queueOutput(" EndGame Message .... failed\n");
    }

    if(pMessage != NULL)
    {
        delete pMessage;
    }

    theEndGame.clearInfo();

}
