
#include <iostream>
using namespace std;


#include "Message.h"
#include "Thread_Output.h"



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
        }
    }

    exitThread_Output();
    


    return 0;    
}

void displayHelp ()
{
    cout << "mwordleclient: Multiplayer Wordle Client" << endl;
    cout << endl;
}


void testsuite_Invoke ()
{
    cout << "Invoking the testing suite" << endl;

    cout << "  Messages - Test Suite" << endl;
    testsuite_Message();
    cout << endl;
}



