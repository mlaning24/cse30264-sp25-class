
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

   protected:

      int   getSocket ()
      { return m_nSocket; }

   public:
      Tracker ();
      ~Tracker();

      bool initialize ();

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

      Message * recvMessage ();

      bool  processEcho (Message * pEchoMessage);
      bool  processRegister (Message * pRegisterMessage);
      bool  processListNodes (Message * pListNodesMessage);
};


#endif