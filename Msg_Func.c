#include "smartsock.h"


void Send_SimpleMsg(int iSock, char * cMsg, unsigned int uiLen)
{
	uCharSize uTxLen;

	uTxLen.iSize = uiLen;
	write(iSock, uTxLen.cSize, 4);

	write(iSock, cMsg, uiLen);

	return;
}

void Send_Room(int iSock, Room * stpRoom)
{
	uCharSize uTxLen;
	char cMsg[MSG_SIZE];

	sprintf(cMsg, "%s: %d", stpRoom->cName, stpRoom->uiCnt);
	uTxLen.iSize = strlen(cMsg);
	write(iSock, uTxLen.cSize, 4);

	write(iSock, cMsg, uTxLen.iSize);

	return;
}

void Send_Msg(int iSock, char * cpID, char * cpMsg, int iMsgLen)
{
	uCharSize uTxLen;

	uTxLen.iSize = iMsgLen + 2 + strlen(cpID);
	write(iSock, uTxLen.cSize, 4);

	write(iSock, cpID, strlen(cpID));
	write(iSock, ": ", 2);
	write(iSock, cpMsg, iMsgLen);

	return;
}

void Send_Signal(int iSock, char * cpFlag)
{
	uCharSize uTxLen;

	uTxLen.iSize = 1;
	write(iSock, uTxLen.cSize, 4);
	write(iSock, cpFlag, 1);

	return;
}
