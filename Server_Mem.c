#include "smartsock.h"


void Init_Room(void)
{
	gb_stpRdRoomHead = (Room *)malloc(sizeof(Room));

	strcpy(gb_stpRdRoomHead->cName, "Waiting");
	gb_stpRdRoomHead->uiCnt = 0;
	gb_stpRdRoomHead->stMem = 0;
	gb_stpRdRoomHead->stNext = 0;

	return;
}

int Client_In(int iCFd, struct sockaddr_in * stAddr)
{
	Member * stpTmpMem;
	Room * stpTmpRoom = gb_stpRoomHead;
	uCharSize RxLen;
	int iRet;

	stpTmpMem = (Member *)malloc(sizeof(Member));
	if( 0 > stpTmpMem )
	{
		return -1;
	}
	stpTmpMem->iFd = iCFd;
	read(iCFd, RxLen.cSize, 4);
	iRet = read(iCFd, stpTmpMem->cName, RxLen.iSize);
	(stpTmpMem->cName)[iRet] = 0;
	stpTmpMem->stPrev = 0;
	stpTmpMem->stNext = 0;

	printf("-------------------------------------\n");
	printf("| Incoming client : %15s |\n", stpTmpMem->cName); 
	printf("| 클라이언트 IP:    %15s |\n", inet_ntoa(stAddr->sin_addr));
	printf("-------------------------------------\n");

	(gb_stpRdRoomHead->uiCnt)++;
	if( gb_stpRdRoomHead->uiCnt > 1 )
	{
		stpTmpMem->stPrev = gb_stpRdMemTail;
		gb_stpRdMemTail->stNext = stpTmpMem;
		gb_stpRdMemTail = stpTmpMem;
	}
	else
	{
		gb_stpRdRoomHead->stMem = stpTmpMem;
		gb_stpRdMemTail = stpTmpMem;
	}

	if(stpTmpRoom == 0)
	{
		Send_SimpleMsg(iCFd, "--   No Room   --", sizeof("--   No Room   --"));
	}
	else
	{
		Send_SimpleMsg(iCFd, "-- Select Room --", sizeof("-- Select Room --"));
	}

	while(stpTmpRoom != 0)
	{
		Send_Room(iCFd, stpTmpRoom);
		stpTmpRoom = stpTmpRoom->stNext;
	}
	Send_SimpleMsg(iCFd, "-----------------", sizeof("-----------------"));

	Send_SimpleMsg(iCFd, "\a", 1);

	return 0;
}

int Client_RoomSelect(Member * stpCurMem)
{
	int iRet;
	uCharSize TxLen;
	Room * stpTmpRoom = gb_stpRoomHead;
	Member * stpTmpMem;
	char cBuf[MSG_SIZE];

	read(stpCurMem->iFd, TxLen.cSize, 4);
	iRet = read(stpCurMem->iFd, cBuf, TxLen.iSize);
	cBuf[iRet] = 0;

	for(iRet = 0; cBuf[iRet] != 0; iRet++)
	{
		cBuf[iRet] = toupper(cBuf[iRet]);
	}

	while(stpTmpRoom != 0)		// 방 찾기
	{
		if( 0 == strcmp(cBuf, stpTmpRoom->cName) )
		{
			// 대기 방에서 나온다.
			(gb_stpRdRoomHead->uiCnt)--;

			if(stpCurMem->stPrev != 0) (stpCurMem->stPrev)->stNext = stpCurMem->stNext;
			else						gb_stpRdRoomHead->stMem = stpCurMem->stNext;
			if(stpCurMem->stNext != 0) (stpCurMem->stNext)->stPrev = stpCurMem->stPrev;

			// 찾은 방에 멤버 추가
			(stpTmpRoom->uiCnt)++;

			(stpTmpRoom->stMem)->stPrev = stpCurMem;
			stpCurMem->stNext = stpTmpRoom->stMem;
			stpTmpRoom->stMem = stpCurMem;

			break;
		}
		stpTmpRoom = stpTmpRoom->stNext;
	}

	if(stpTmpRoom == 0)	// 방이 없으면 방을 만든다
	{
		(gb_stpRdRoomHead->uiCnt)--;

		if(stpCurMem->stPrev != 0) (stpCurMem->stPrev)->stNext = stpCurMem->stNext;
		else						gb_stpRdRoomHead->stMem = stpCurMem->stNext;
		if(stpCurMem->stNext != 0) (stpCurMem->stNext)->stPrev = stpCurMem->stPrev;

		stpTmpRoom = (Room *)malloc(sizeof(Room));
		strcpy(stpTmpRoom->cName, cBuf);
		stpTmpRoom->uiCnt = 1;
		stpTmpRoom->stMem = stpCurMem;

		stpTmpRoom->stPrev = 0;
		if( gb_stpRoomHead != 0 )
		{
			stpTmpRoom->stNext = gb_stpRoomHead;
			gb_stpRoomHead->stPrev = stpTmpRoom;
			gb_stpRoomHead = stpTmpRoom;
		}
		else
		{
			gb_stpRoomHead = stpTmpRoom;
			stpTmpRoom->stNext = 0;
		}
	}

	sprintf(cBuf, "-------------- %s --------------", stpTmpRoom->cName);
	iRet = strlen(cBuf);
	Send_SimpleMsg(stpCurMem->iFd, cBuf, iRet);

	sprintf(cBuf, "[%s]님이 채팅방에 입장하셨습니다.", stpCurMem->cName);
	iRet = strlen(cBuf);
	for( stpTmpMem = stpTmpRoom->stMem; stpTmpMem != 0; stpTmpMem = stpTmpMem->stNext)
	{
		Send_SimpleMsg(stpTmpMem->iFd, cBuf, iRet);
	}

	printf(">>  Client[%s] to Room[%s]\n", stpCurMem->cName, stpTmpRoom->cName);
	return 0;
}

int Client_MsgRx(Room * stpCurRoom, Member * stpCurMem)
{
	int iRet;
	uCharSize TxLen;
	Room * stpTmpRoom = gb_stpRoomHead;
	Member * stpTmpMem;
	char cBuf[MSG_SIZE];

	read(stpCurMem->iFd, TxLen.cSize, 4);
	iRet = read(stpCurMem->iFd, cBuf, TxLen.iSize);
	if( (iRet == 0)
			||
			(strcmp(MSG_END, cBuf) == 0)
	  )
	{
		Client_Out(stpCurRoom, stpCurMem);
		return 1;
	}
	cBuf[iRet] = 0;

	for( stpTmpMem = stpCurRoom->stMem; stpTmpMem != 0; stpTmpMem = stpTmpMem->stNext )
	{
		if(stpTmpMem == stpCurMem)
		{
			continue;
		}
		Send_SimpleMsg(stpTmpMem->iFd, cBuf, strlen(cBuf));
	}

	return 0;
}

int Client_Out(Room * stpCurRoom, Member * stpCurMem)
{
	char cBuf[MSG_SIZE];
	unsigned int uiLen;
	Member * stpTmpMem;

	(stpCurRoom->uiCnt)--;
	// 멤버 삭제
	if(stpCurMem->stPrev != 0) (stpCurMem->stPrev)->stNext = stpCurMem->stNext;
	else                        stpCurRoom->stMem = stpCurMem->stNext;
	if(stpCurMem->stNext != 0) (stpCurMem->stNext)->stPrev = stpCurMem->stPrev;

	// 방의 멤버가 없으면 방 삭제
	if( stpCurRoom == gb_stpRdRoomHead )
	{
		return 0;
	}

	if(stpCurRoom->uiCnt == 0)
	{
		if(stpCurRoom->stPrev != 0) (stpCurRoom->stPrev)->stNext = stpCurRoom->stNext;
		else                        gb_stpRoomHead = stpCurRoom->stNext;
		if(stpCurRoom->stNext != 0) (stpCurRoom->stNext)->stPrev = stpCurRoom->stPrev;

		write(1, ">> ", sizeof(">> "));
		sprintf(cBuf, "채팅방 [%s]이 소멸되었습니다. \n", stpCurRoom->cName);
		write(0, cBuf, strlen(cBuf));

		free(stpCurRoom);
	}
	else
	{
		sprintf(cBuf, "[%s]님이 퇴장하셨습니다.", stpCurMem->cName);
		uiLen = strlen(cBuf);
		for( stpTmpMem = stpCurRoom->stMem; stpTmpMem != 0; stpTmpMem = stpTmpMem->stNext)
		{
			if(stpTmpMem == stpCurMem)
			{
				continue;
			}
			Send_SimpleMsg(stpTmpMem->iFd, cBuf, uiLen);
		}

		write(1, ">> ", sizeof(">> "));
		write(1, cBuf, uiLen);
		putchar('\n');
	}
	free(stpCurMem);

	return 0;
}


