#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#define MULTICASTIP "235.7.8.9"
#define REMOTEPORT  9000
#define BUFSIZE     512

// 소켓 함수 오류 출력 후 종료
void err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// 소켓 함수 오류 출력
void err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}
enum class PROTOCOL
{
	NONE = 0,
	MAIN,
	ADDRESS,
	CHAT
};
struct Packet
{
	PROTOCOL protocol;
	int data;
	char buf[BUFSIZE];
};
int main(int argc, char *argv[])
{
	int retval;
	// 윈속 초기화
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock == INVALID_SOCKET) err_quit("socket()");
	
	// 소켓 주소 구조체 초기화
	SOCKADDR_IN remoteaddr;
	ZeroMemory(&remoteaddr, sizeof(remoteaddr));
	remoteaddr.sin_family = AF_INET;
	remoteaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	remoteaddr.sin_port = htons(REMOTEPORT);
	retval = bind(sock, (SOCKADDR*)&remoteaddr, sizeof(remoteaddr));
	if (retval == SOCKET_ERROR)
	{
		err_display("bind()");
	}
	SOCKADDR_IN peeraddr;
	ZeroMemory(&peeraddr, sizeof(peeraddr));
	// 데이터 통신에 사용할 변수
	char buf[BUFSIZE+1];
	int len=sizeof(peeraddr);
	//방마다 설정해줄 ip주소 설정
	SOCKADDR_IN roomaddr[3];
	for(int i=0;i<3;++i)
	{
		ZeroMemory(&roomaddr[i], sizeof(SOCKADDR_IN));
		roomaddr[i].sin_family = AF_INET;
		ZeroMemory(buf, BUFSIZE + 1);
		sprintf(buf, "224.1.1.%d", i);
		roomaddr[i].sin_addr.s_addr = inet_addr(buf);
		roomaddr[i].sin_port = htons(REMOTEPORT);
	}
	

	Packet packet;
	ZeroMemory(&packet, sizeof(Packet));
	char username[BUFSIZE];
	ZeroMemory(username, BUFSIZE);
	// 멀티캐스트 데이터 보내기
	while(1)
	{
		bool first = false;
		ZeroMemory(buf, BUFSIZE + 1);
		SOCKADDR_IN tempaddr;
		ZeroMemory(&tempaddr, BUFSIZE);
		retval=recvfrom(sock,(char*)&packet,sizeof(Packet),0,(SOCKADDR*)&peeraddr,&len);
		if (retval == SOCKET_ERROR)
		{
			err_display("recvfrom()");
		}
		switch (packet.protocol)
		{
		case PROTOCOL::MAIN:
			strcpy(buf, "방선택\n1.박용택\n2.오지환\n3.김현수\n");
			retval=sendto(sock, buf, BUFSIZE+1, 0,
				(SOCKADDR*)&peeraddr, sizeof(peeraddr));
			if (retval == SOCKET_ERROR)
			{
				err_display("sendto()");
				continue;
			}
			break;
		case PROTOCOL::ADDRESS:
			ZeroMemory(buf, BUFSIZE + 1);
			strcpy(username, packet.buf);
			printf("[UDP IP:%s|PORT:%d] %s 유저 %d번방 입장\n",inet_ntoa(peeraddr.sin_addr),ntohs(peeraddr.sin_port),username, packet.data);
			switch (packet.data)
			{
			case 1:
				memcpy(&tempaddr, &roomaddr[0], sizeof(roomaddr[0]));
				break;
			case 2:
				memcpy(&tempaddr, &roomaddr[1], sizeof(roomaddr[1]));
				break;
			case 3:
				memcpy(&tempaddr, &roomaddr[2], sizeof(roomaddr[2]));
				break;
			}
			retval = sendto(sock, (char*)&tempaddr, sizeof(tempaddr), 0, (SOCKADDR*)&peeraddr, sizeof(peeraddr));
			break;
		case PROTOCOL::CHAT:
			
			break;
		}
		//// 데이터 입력
		//printf("\n[보낼 데이터] ");
		//if(fgets(buf, BUFSIZE+1, stdin) == NULL)
		//	break;

		//// '\n' 문자 제거
		//len = strlen(buf);
		//if(buf[len-1] == '\n')
		//	buf[len-1] = '\0';
		//if(strlen(buf) == 0)
		//	break;

		//// 데이터 보내기
		//retval = sendto(sock, buf, strlen(buf), 0,
		//	(SOCKADDR *)&remoteaddr, sizeof(remoteaddr));
		//if(retval == SOCKET_ERROR)
		//{
		//	err_display("sendto()");
		//	continue;
		//}
		//printf("[UDP] %d바이트를 보냈습니다.\n", retval);
	}

	// closesocket()
	closesocket(sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}