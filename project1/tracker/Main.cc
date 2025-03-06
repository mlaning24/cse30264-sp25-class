
#include <iostream>
using namespace std;

#include <stdlib.h>

#include "Tracker.h"


void showHelp ()
{
   cout << "Usage: tracker port debug" << endl;
   cout << "  port   The port number on which to receive UDP packets" << endl;
   cout << "  debug  Turn on extra verbose debugging" << endl;
}


int main (int argc, char** argv)
{
   if (argc != 2 && argc != 3) {
      showHelp();
      exit(-1);
   }

   /* Register / properly shutdown on a Control-C */

   Tracker  theTracker;

   /* Take the port from the first argument */
   /*   Is this properly protected? */
   theTracker.setPort(atoi(argv[1]));

   /* Try to initialize the tracker */
   if (!theTracker.initialize()) {
      cerr << "Error: Failed to initializer tracker - exiting" << endl;
      exit(-1);
   }

   /* Sit in a loop that really just keeps going anyway*/
   while(1)
   {
      theTracker.go();
   }
}