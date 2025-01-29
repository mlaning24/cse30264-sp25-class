
#include <iostream>
using namespace std;


#include "Server.h"
#include "Message.h"
#include "Thread_Output.h"

// The global / big boss server
Server g_theServer;


void displayHelp ();
void testsuite_Invoke ();



int main (int argc, char** argv)
{
    cout << "Argument Count: " << argc << endl;


    initializeThread_Output();

    // Single case where the only argument is to test
    if(argc == 2)
    {
        if(strcmp(argv[1], "-test") == 0)
        {
            testsuite_Invoke();
            exitThread_Output();
            exit(0);
        }
    }

    int nNextArg = 1;

    /* Process the arguments starting at the first argument 
       after the executable itself (that is [0]) */
    while(nNextArg < argc)
    {
        if(strcmp(argv[nNextArg], "-np") == 0)
        {
            if(nNextArg + 1 >= argc)
            {
                queueOutput("Error: No value given for the number of players (-np)\n");                
                exitThread_Output();                
                exit(-1);
            }

            int nPlayers;

            nPlayers = atoi(argv[nNextArg+1]);

            if(nPlayers <= 0)
            {
                queueOutput("Error: Number of players specified must be at least 1\n");
                exitThread_Output();                
                exit(-1);
            }

            if(nPlayers > SERVER_MAX_PLAYERS)
            {
                char szBuffer[100];

                sprintf(szBuffer, "Error: Number of players exceeds the limit (%d)\n", SERVER_MAX_PLAYERS);
                queueOutput(szBuffer);
                exitThread_Output();                
                exit(-1);
            }

            g_theServer.setPlayersPerGame(nPlayers);
            nNextArg += 2;
        }

        if(strcmp(argv[nNextArg], "-lp") == 0)
        {
            if(nNextArg + 1 >= argc)
            {
                queueOutput("Error: No value given for the lobby port (-lp)\n");                
                exitThread_Output();                
                exit(-1);
            }

            int nLobby;

            nLobby = atoi(argv[nNextArg+1]);

            if(nLobby <= 0)
            {
                queueOutput("Error: Lobby port must be greater than zero\n");
                exitThread_Output();                
                exit(-1);
            }

            if(nLobby > SERVER_MAX_LOBBY_PORT)
            {
                char szBuffer[100];

                sprintf(szBuffer, "Error: Lobby port exceeds the limit (%d)\n", SERVER_MAX_LOBBY_PORT);
                queueOutput(szBuffer);
                exitThread_Output();                
                exit(-1);
            }

            g_theServer.setPlayersPerGame(nPlayers);
        }

    }


    exit(0);

    /* Start up the server and handle input here via the console */
    g_theServer.start();

    string  sInput;

    while(1)
    {
        queueOutput(">");
        cin >> sInput;

        if(sInput == "quit")
        {
            break;
        }
    }

    g_theServer.stop();


    exitThread_Output();
    


    return 0;    
}

void displayHelp ()
{
    cout << "mwordleserver: Multiplayer Wordle Server" << endl;
    cout << endl;
}


void testsuite_Invoke ()
{
    cout << "Invoking the testing suite" << endl;

    cout << "  Messages - Test Suite" << endl;
    testsuite_Message();
    cout << endl;
}



