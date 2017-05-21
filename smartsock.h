#ifndef __SMARTSOCK_H__
#define __SMARTSOCK_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
// socket & bind & listen & accept & connect
#include <sys/types.h>
#include <sys/socket.h>

// sockaddr_in
#include <netinet/in.h>
// read & write
#include <unistd.h>
// htonl
#include <arpa/inet.h>
// errno, perror
#include <errno.h>
// open
#include <fcntl.h>
#include <sys/stat.h>
// select
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>

#include <ctype.h>

#define PORT    	7779			// Server PORT
#define IP      	"192.168.0.161"	// Server IP

#define MAX_USER		3
#define NIC_NAME_SIZE	9
#define NIC_NAME_MSG	(9 + 2)
#define ROOM_NAME_SIZE	10

#define BUF_SIZE	255
#define MSG_SIZE	(BUF_SIZE + 1 + NIC_NAME_MSG)

#define MSG_END		"\x01\x02\x03"
#define MSG_START	"\x04"
#define MSG_AGAIN	"\x05"
#define MSG_GET_ROOM	"HELP"

typedef struct _mem {
	int iFd;
	char cName[NIC_NAME_SIZE];
	struct _mem * stPrev;
	struct _mem * stNext;
} Member;

typedef struct _room {
	char cName[ROOM_NAME_SIZE];
	unsigned int uiCnt;
	struct _mem * stMem;
	struct _room * stPrev;
	struct _room * stNext;
} Room;

typedef union _size
{
	char cSize[4];
	int iSize;
}   uCharSize;

extern Room * gb_stpRdRoomHead;
extern Member * gb_stpRdMemTail;
extern Room * gb_stpRoomHead;

#include "Server_Mem.h"
#include "Msg_Func.h"

#endif /* __SMARTSOCK_H__ */
