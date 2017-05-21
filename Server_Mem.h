#ifndef __SERVER_MEM_H__
#define __SERVER_MEM_H__

void Init_Room(void);
int Client_In(int iCFdi, struct sockaddr_in * stAddr);
int Client_RoomSelect(Member * stpCurMem);
int Client_Out(Room * stpCurRoom, Member * stpCurMem);
int Client_MsgRx(Room * stpCurRoom, Member * stpCurMem);

#endif /* __SERVER_MEM_H__ */
