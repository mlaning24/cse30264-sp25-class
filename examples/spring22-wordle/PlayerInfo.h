
#ifndef __PLAYER_INFO_H
#define __PLAYER_INFO_H

#include <string>
using namespace std;

#include <time.h>


class PlayerInfo 
{
    private:
        string  m_sName;                // Player name
        int     m_nNumber;              // Player number
        int     m_nScore;               // Score or earned
        bool    m_bCorrectWinner;       // Correct or winner
        timeval m_ReceiptTime;          // Receipt time 
        string  m_sResult;              // Result string for the guess

    public:
        PlayerInfo ();

        void    setName (string sName);
        string  getName ();

        void    setNumber (string sNumber);
        void    setNumber (int nNumber);
        int     getNumber ();
        string  getNumberAsString ();        


        void    setScore (string sScore);
        void    setScore (int nScore);
        int     getScore ();        
        string  getScoreAsString ();

        void    setWinner (string sWinner);
        void    setWinner (bool bWinner);
        bool    getWinner ();     
        string  getWinnerAsString();   

        void    setResult (string sResult);
        string  getResult ();

        void    setReceiptTime (double fReceiptTime);
        void    setReceiptTime (timeval theTime);
        void    setReceiptTime (string sReceiptTime);
        timeval getReceiptTime (); 
        string  getReceiptTimeAsString ();       
};



#endif
