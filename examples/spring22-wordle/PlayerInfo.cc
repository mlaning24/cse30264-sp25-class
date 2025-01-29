
#include <stdlib.h>
#include <math.h>

#include "PlayerInfo.h"

PlayerInfo::PlayerInfo ()
{
    m_sName = "";
    m_nNumber = -1;
    m_nScore = -1;
    m_bCorrectWinner = false;
    m_sResult = "";
}

void    PlayerInfo::setName (string sName)
{
    m_sName = sName;
}

string  PlayerInfo::getName ()
{
    return m_sName;
}
void    PlayerInfo::setNumber (string sNumber)
{
    sscanf(sNumber.c_str(), "%d", &m_nNumber);    
}

void    PlayerInfo::setNumber (int nNumber)
{
    m_nNumber = nNumber;
}

int     PlayerInfo::getNumber ()
{
    return m_nNumber;
}

string  PlayerInfo::getNumberAsString ()
{
    char szBuffer[20];

    sprintf(szBuffer, "%d", m_nNumber);

    return szBuffer;
}


void    PlayerInfo::setScore (string sScore)
{
    sscanf(sScore.c_str(), "%d", &m_nScore);    
}

void    PlayerInfo::setScore (int nScore)
{
    m_nScore = nScore;
}

int     PlayerInfo::getScore ()
{
    return m_nScore;
}   

string  PlayerInfo::getScoreAsString ()
{
    char szBuffer[24];

    sprintf(szBuffer, "%d", m_nScore);

    return szBuffer;
}

void    PlayerInfo::setWinner (string sWinner)
{
    if(sWinner == "Yes")
    {
        m_bCorrectWinner = true;
    }
    else 
    {
        m_bCorrectWinner = false;
    }
}

void    PlayerInfo::setWinner (bool bWinner)
{
    m_bCorrectWinner = bWinner;
}

bool    PlayerInfo::getWinner ()
{
    return m_bCorrectWinner;
}

string  PlayerInfo::getWinnerAsString()
{
    if(m_bCorrectWinner)
    {
        return "Yes";
    }
    else 
    {
        return "No";
    }
}

void    PlayerInfo::setResult (string sResult)
{
    m_sResult = sResult;
}

string  PlayerInfo::getResult ()
{
    return m_sResult;
}

void    PlayerInfo::setReceiptTime (timeval theTime)
{
    m_ReceiptTime = theTime;
}

void    PlayerInfo::setReceiptTime (double fReceiptTime)
{
    m_ReceiptTime.tv_sec = trunc(fReceiptTime);

    double  fDecimal;

    modf(fReceiptTime, &fDecimal);
    fDecimal *= 1000000;    
}

void    PlayerInfo::setReceiptTime (string sReceiptTime)
{
    sscanf(sReceiptTime.c_str(), "%ld.%d", &(m_ReceiptTime.tv_sec), &(m_ReceiptTime.tv_usec));

    // Needs a fix when the decimal place value is less than six digits
    //  This is a very hack-ish fix

    if(m_ReceiptTime.tv_usec < 10)
    {
        m_ReceiptTime.tv_usec *= 100000;
    }
    else if(m_ReceiptTime.tv_usec < 100)
    {
        m_ReceiptTime.tv_usec *= 10000;
    }
    else if (m_ReceiptTime.tv_usec < 1000)
    {
        m_ReceiptTime.tv_usec *= 1000;
    }
    else if (m_ReceiptTime.tv_usec < 10000)
    {
        m_ReceiptTime.tv_usec *= 100;
    }
    else if (m_ReceiptTime.tv_usec < 100000)
    {
        m_ReceiptTime.tv_usec *= 10;
    }
    else 
    {
        // Nothing - all good as we were greater than 100,000
    }
}

timeval  PlayerInfo::getReceiptTime ()
{
    return m_ReceiptTime;
}

string  PlayerInfo::getReceiptTimeAsString ()
{
    char    szBuffer[32];

    // Print it out as seconds.microseconds
    sprintf(szBuffer, "%ld.%d", m_ReceiptTime.tv_sec, m_ReceiptTime.tv_usec);
    return szBuffer;
}      
