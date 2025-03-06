
#include "Message.h"

Message::Message ()
{
   m_byType = MSG_TYPE_UNKNOWN;
   m_nDataLength = 0;
   memset(m_byData, MSG_MAX_SIZE, 0);
}

Message::~Message ()
{

}

string Message::getTypeAsString ()
{
   switch(getType())
   {
      case MSG_TYPE_UNKNOWN:
         return "Unknown";
      case MSG_TYPE_REGISTER:
         return "Register";
      case MSG_TYPE_REGISTER_ACK:
         return "Register-Ack";
      case MSG_TYPE_LIST_NODES:
         return "List-Nodes";
      case MSG_TYPE_LIST_NODES_DATA:
         return "List-Nodes-Data";
      case MSG_TYPE_ECHO:
         return "Echo";
      case MSG_TYPE_ECHO_RESPONSE:
         return "Echo-Response";
   }
}
