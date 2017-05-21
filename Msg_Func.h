#ifndef __MSG_FUNC_H__
#define __MSG_FUNC_H__

void Send_SimpleMsg(int iSock, char * cMsg, unsigned int uiLen);
void Send_Room(int iSock, Room * stpRoom);
int Split_Buffer(char * Buffer, char * ID, char * Msg);
void Send_Msg(int iSock, char * cpID, char * cpMsg, int iMsgLen);
void Send_Signal(int iSock, char * cpFlag);

#endif /* __MSG_FUNC_H__ */
