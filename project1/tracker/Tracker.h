
#ifndef __TRACKER_H
#define __TRACKER_H

#include <vector>
using namespace std;

#include <stdint.h>

#include "Node.h"
#include "Message.h"

class Tracker
{
   private:
      vector<Node> m_NodeTable;

      // The port that the tracker will be bound
      uint16_t m_nPort;

      bool     m_bVerbose;

      // The socket associated with the tracker (server)
      int      m_nSocket;

      /* What is our particular information for the server? */
      struct sockaddr_in m_AddressInfo;



   public:

      Tracker ();
      ~Tracker();

      bool initialize (char * pszIP);

      struct sockaddr_in * getAddressInfo ()
      { return &m_AddressInfo; }

      int   getSocket ()
      { return m_nSocket; }

      uint16_t getPort ()
      { return m_nPort; }

      void setPort (uint16_t nPort)
      { m_nPort = nPort; }

      bool isVerbose ()
      { return m_bVerbose; }

      void setVerbose (bool bVerbose)
      { m_bVerbose = bVerbose; }

      /* Sit and loop */
      void  go ();

      /** Wait for a message on the server socket
       * @returns Pointer to a valid object if there was a message successfully read
       */
      Message * recvMessage ();

      bool  processEcho (Message * pEchoMessage);
      bool  processRegister (Message * pRegisterMessage);
      bool  processListNodes (Message * pListNodesMessage);

      bool  doEcho ();

};


#endif