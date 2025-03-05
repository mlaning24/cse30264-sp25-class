// Message.h : Message container for datagrams to / from the tracker

#ifndef __MESSAGE_H
#define __MESSAGE_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>


#define MSG_MAX_SIZE    1500

#define MSG_TYPE_UNKOWN          0
#define MSG_TYPE_REGISTER        1
#define MSG_TYPE_REGISTER_ACK    2
#define MSG_TYPE_LIST_NODES      3
#define MSG_TYPE_LIST_NODES_DATA 4

class Message
{
   private:
      uint8_t     m_byType;
      uint16_t    m_nDataLength;
      uint8_t     m_byData [MSG_MAX_SIZE];

   public:
      /** Default constructor */
      Message ()
      {
         m_byType = MSG_TYPE_UNKOWN;
         m_nDataLength = 0;
         memset(m_byData, MSG_MAX_SIZE, 0);
      }

      uint8_t  getType ()
      { return m_byType; }

      void setType (uint8_t byType)
      { m_byType = byType; }
};

#endif
