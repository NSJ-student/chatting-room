#include "smartsock.h"

Room * gb_stpRdRoomHead;
Member * gb_stpRdMemTail;
Room * gb_stpRoomHead;

int main(int iRet, char * cpCmd[])	// 인자를 받는 main 함수
{
	unsigned short usPort = PORT;

	fd_set fdRead;
	int iSock;
	int iCSock;
	int iMSock;

	unsigned int uiCnt;
	unsigned int uiCnt2;

	char cBuf[BUF_SIZE];
	char cMsg[MSG_SIZE];
	char cNic[MAX_USER][NIC_NAME_SIZE];

	socklen_t uiSocklen = sizeof(struct sockaddr);
	struct sockaddr_in stAddr;

	Member * stTmpMem = 0;
	Member * stFreeMem = 0;
	Room * stTmpRoom = 0;
	Room * stFreeRoom = 0;

	if( 2 == iRet )		// 서버의 포트번호 설정
	{
		iSock = atoi(cpCmd[1]);
		if( 1024 < iSock )
		{
			if( iSock < 65535 )
			{
				usPort = iSock;
			}
		}
	}

	printf("Port : %d\n", usPort);

	iSock = socket(AF_INET, SOCK_STREAM, 0);
	if(iSock < 0)
	{
		perror("socket() : ");
		return 10;
	}

	bzero(&stAddr, sizeof(stAddr));
	stAddr.sin_family = AF_INET;
	stAddr.sin_addr.s_addr = inet_addr(IP);
	stAddr.sin_port = htons(usPort);

	iRet = bind(iSock, (struct sockaddr *)&stAddr, sizeof(stAddr));
	if(iRet < 0)
	{
		perror("bind() : ");
		close(iSock);
		return 20;
	}

	iRet = listen(iSock, 5);
	if(iRet < 0)
	{
		perror("listen() : ");
		close(iSock);
		return 30;
	}

	Init_Room();

	while(1)
	{
		FD_ZERO(&fdRead);
		FD_SET(0, &fdRead);		// 키보드 입력
		FD_SET(iSock, &fdRead);	// 소켓 접속
		iMSock = iSock;
		for( stTmpRoom = gb_stpRoomHead; stTmpRoom != 0; stTmpRoom = stTmpRoom->stNext)
		{
			for(stTmpMem = stTmpRoom->stMem; stTmpMem != 0; stTmpMem = stTmpMem->stNext)
			{
				FD_SET(stTmpMem->iFd, &fdRead);
				if( iMSock < stTmpMem->iFd )
				{
					iMSock = stTmpMem->iFd;
				}
			}
		}
		for(stTmpMem = gb_stpRdRoomHead->stMem; stTmpMem != 0; stTmpMem = stTmpMem->stNext)
		{
			FD_SET(stTmpMem->iFd, &fdRead);
			if( iMSock < stTmpMem->iFd )
			{
				iMSock = stTmpMem->iFd;
			}
		}


		select(iMSock+1, &fdRead, 0, 0, 0);

		if( 0 != FD_ISSET(iSock, &fdRead) )	// 신규 클라이언트 접속
		{
			iCSock = accept(iSock, (struct sockaddr *)&stAddr, &uiSocklen);
			if(iCSock < 0)
			{
				perror("accept() : ");
				continue;
			}

			if( 0 > Client_In(iCSock, &stAddr))
			{
				perror("Client_In: ");
				return 0;
			}
		}
		if( 0 != FD_ISSET(0, &fdRead) )		// 서버 입력
		{
			iRet = read(0, cBuf, BUF_SIZE);			// 콘솔 입력을 읽는다

			if( iRet == 0 ) // Ctrl+D 입력
			{
				sprintf( cMsg, "Server is ending", sizeof("Server is ending") );
				iRet = strlen(cMsg);
				for( stTmpRoom = gb_stpRoomHead; stTmpRoom != 0; stTmpRoom = stTmpRoom->stNext)
				{
					for(stTmpMem = stTmpRoom->stMem; stTmpMem != 0; stTmpMem = stTmpMem->stNext)
					{
						Send_SimpleMsg(stTmpMem->iFd, cMsg, iRet);
					}
				}

				break;
			}

			cBuf[iRet - 1] = 0;
			sprintf(cMsg, "공지사항: [%s]", cBuf);
			iRet = strlen(cBuf);
			for( stTmpRoom = gb_stpRoomHead; stTmpRoom != 0; stTmpRoom = stTmpRoom->stNext)
			{
				for(stTmpMem = stTmpRoom->stMem; stTmpMem != 0; stTmpMem = stTmpMem->stNext)
				{
					Send_SimpleMsg(stTmpMem->iFd, cBuf, iRet);
				}
			}
		}
		for( stTmpRoom = gb_stpRoomHead; stTmpRoom != 0; stTmpRoom = stTmpRoom->stNext )	// 접속한 모든 클라이언트 검사
		{
			for(stTmpMem = stTmpRoom->stMem; stTmpMem != 0; stTmpMem = stTmpMem->stNext)
			{
				if( 0 != FD_ISSET(stTmpMem->iFd, &fdRead) )	// 클라이언트 입력 수신
				{
					Client_MsgRx(stTmpRoom, stTmpMem);
				}
			}
		}
		// 대기 중인 클라이언트 방 선택
		for( stTmpMem = gb_stpRdRoomHead->stMem; stTmpMem != 0; stTmpMem = stTmpMem->stNext)
		{
			if( 0 != FD_ISSET(stTmpMem->iFd, &fdRead) )
			{
				Client_RoomSelect(stTmpMem);
			}
		}
	}

	for( stTmpRoom = gb_stpRoomHead; stTmpRoom != 0; )
	{
		for(stTmpMem = stTmpRoom->stMem; stTmpMem != 0;)
		{
			close(stTmpMem->iFd);
			stFreeMem = stTmpMem;
			stTmpMem = stTmpMem->stNext;
			free(stFreeMem);
		}
		stFreeRoom = stTmpRoom;
		stTmpRoom = stTmpRoom->stNext;
		free(stFreeRoom);
	}

	close(iSock);   // 랑데뷰 소켓 닫기
	return 0;
}
