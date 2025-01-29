
#include <pthread.h>

#include <iostream>
#include <string>
#include <vector>
#include <queue>
using namespace std;

#include "Thread_Output.h"

/* Global variables for the output thread */
pthread_mutex_t     g_Mutex_Output;
pthread_cond_t      g_Cond_Output;
pthread_t           g_Thread_Output;
bool                g_bOutput_KeepGoing;
queue<string>       g_Queue_Output;


void queueOutput (string sOutput)
{
    pthread_mutex_lock(&g_Mutex_Output);
    g_Queue_Output.push(sOutput);
    pthread_cond_signal(&g_Cond_Output);
    pthread_mutex_unlock(&g_Mutex_Output);
}

void initializeThread_Output ()
{
    /* Configure the output to keep looping */
    g_bOutput_KeepGoing = true;

    /* Initialize the mutex for protecting the queue */
    pthread_mutex_init(&g_Mutex_Output, NULL);

    /* Initialize the condition variable */
    pthread_cond_init(&g_Cond_Output, NULL);

    /* Start up the thread that does the actual ordered output */
    pthread_create(&g_Thread_Output, NULL, Thread_Output, NULL);
}

/** Wait for the output thread to complete */
void exitThread_Output ()
{
    g_bOutput_KeepGoing = false;
    pthread_cond_signal(&g_Cond_Output);

    //cout << "Waiting for the thread to wrap up" << endl;
    pthread_join(g_Thread_Output, NULL);
}

/** Thread for output to the console via cout */
void * Thread_Output (void *)
{
    while(g_bOutput_KeepGoing)
    {
        /* Fetch the lock for the output queue */
        pthread_mutex_lock(&g_Mutex_Output);

        while(g_Queue_Output.size() == 0)
        {
            /* Give up the lock and wait on the conditional variable */
            pthread_cond_wait(&g_Cond_Output, &g_Mutex_Output);            

            /* Handle the special case where we are signaled to exit but 
               there is not anything else remaining to output */
            if(g_Queue_Output.size() == 0 && !g_bOutput_KeepGoing)
            {
                pthread_mutex_unlock(&g_Mutex_Output);
                return NULL;
            }
        }

        /* If we got here - we have the lock and there is something to output */
        while(g_Queue_Output.size() > 0)
        {
            cout << g_Queue_Output.front();
            g_Queue_Output.pop();
        }

        pthread_mutex_unlock(&g_Mutex_Output);
    }

    return NULL;
}
