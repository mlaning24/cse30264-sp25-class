
#ifndef __THREAD_OUTPUT_H
#define __THREAD_OUTPUT_H

/** Queue a string for output with thread safety 
 * @param sOutput The string to queue for output 
 */
void queueOutput (string sOutput);

/** Initialize the output thread */
void initializeThread_Output ();

/** Exit and wait for the output thread to complete */
void exitThread_Output ();

/** Thread for output to the console via cout */
void * Thread_Output (void *);


#endif