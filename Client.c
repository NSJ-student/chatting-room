#include "smartsock.h"


int main(int iRet, char * cpCmd[])
{
	unsigned short usPort = PORT;

	int iFd;
	struct sockaddr_in stAddr;
	int iLen;
	fd_set fdRead;
	uCharSize TxLen;
	char cBuf[BUF_SIZE];
	char cMsg[MSG_SIZE];
	char cNic[NIC_NAME_SIZE];

	if( 2 == iRet )     // 서버 포트번호 설정
	{
		iFd = atoi(cpCmd[1]);
		if( 1024 < iFd )
		{
			if( iFd < 65535 )
			{
				usPort = iFd;
			}
		}
	}

	printf("Port : %d\n", usPort);


	// 닉네임 입력
	while(1)
	{
		printf("Input Nickname : ");
		fflush(stdout);

		iRet = read(0, cNic, NIC_NAME_SIZE);
		if( iRet < 2 )
		{
			continue;	// Enter와 Ctrl+D를 방지
		}

		cNic[iRet-1] = 0;
		// fflush(stdin);	// gcc에서는 입력버퍼를 fflush로 비울 수 없다
		break;
	}

	iFd = socket(AF_INET, SOCK_STREAM, 0);
	if( -1 == iFd )
	{
		perror("socket() call error : ");
		return 100;
	}

	stAddr.sin_family = AF_INET;
	stAddr.sin_addr.s_addr = inet_addr(IP);
	stAddr.sin_port = htons(usPort);

	iLen = sizeof(struct sockaddr_in);

	iRet = connect(iFd, (struct sockaddr *)&stAddr, iLen);
	if( -1 == iRet )
	{
		perror("connect() call error : ");
		close(iFd);
		return 200;
	}

	TxLen.iSize = strlen(cNic);
	write(iFd, TxLen.cSize, 4);
	write(iFd, cNic, TxLen.iSize);

	while(1)
	{
		read(iFd, TxLen.cSize, 4);
		iRet = read(iFd, cMsg, TxLen.iSize);
		if( iRet == 1 )
		{
			break;
		}
		cMsg[iRet] = 0;
		printf("%s\n", cMsg);
	}

	iRet = read(0, cBuf, ROOM_NAME_SIZE);
	cBuf[iRet-1] = 0;
	TxLen.iSize = strlen(cBuf);
	write(iFd, TxLen.cSize, 4);
	write(iFd, cBuf, TxLen.iSize);

	read(iFd, TxLen.cSize, 4);
	iRet = read(iFd, cMsg, TxLen.iSize);
	cMsg[iRet] = 0;

	while(1)
	{
		FD_ZERO(&fdRead);
		FD_SET(0, &fdRead);
		FD_SET(iFd, &fdRead);

		select(iFd+1, &fdRead, 0, 0, 0);	// select에서 키보드 입력과 iFd를 감시한다
		// 키보드나 socket 입력이 있으면 blocking이 풀린다

		if( 0 != FD_ISSET(0, &fdRead) )		// 키보드 입력을 감지
		{
			iRet = read(0, cBuf, BUF_SIZE);
			if( iRet == 0 )	// Ctrl+D 입력
			{
				TxLen.iSize = sizeof(MSG_END);
				write(iFd, MSG_END, sizeof(MSG_END));

				break;
			}

			cBuf[iRet-1] = 0;

			sprintf(cMsg, "[%s] %s", cNic, cBuf);
			TxLen.iSize = strlen(cMsg);
			write(iFd, TxLen.cSize, 4);
			write(iFd, cMsg, TxLen.iSize);
		}
		if( 0 != FD_ISSET(iFd, &fdRead) )
		{
			read(iFd, TxLen.cSize, 4);
			iRet = read(iFd, cMsg, TxLen.iSize);
			cMsg[iRet] = 0;

			if( 0 == iRet )	// 서버 연결이 끊김
			{
				printf("Server Error\n");
				break;
			}

			if( 0 == strncmp( MSG_END, cMsg, sizeof(MSG_END)) )
			{
				break;
			}

			printf("%s\n", cMsg);
		}
	}

	close(iFd);
	return 0;
}
