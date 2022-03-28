#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#define MULTICASTIP "235.7.8.9"
#define LOCALPORT   9000
#define BUFSIZE     512

// 소켓 함수 오류 출력 후 종료
void err_quit(char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// 소켓 함수 오류 출력
void err_display(char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char*)lpMsgBuf);
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
DWORD WINAPI RecvThread(LPVOID lpParam)
{
	SOCKET sock;
	SOCKADDR_IN peeraddr1;
	ZeroMemory(&peeraddr1, sizeof(peeraddr1));
	int addrlen = sizeof(peeraddr1);
	int retval = 0;
	char buf[BUFSIZE];
	//while (1)
	{
		ZeroMemory(buf, BUFSIZE);
		retval = recvfrom(sock, (char*)buf, BUFSIZE, 0,
			NULL,0);
		if (retval == SOCKET_ERROR)
		{
			err_display("recvfrom()");
		}
		printf("%s\n", buf);
	}
	return 0;
}
int main(int argc, char* argv[])
{
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");
	
	// SO_REUSEADDR 옵션 설정
	/*BOOL optval = TRUE;
	retval = setsockopt(sock, SOL_SOCKET,
		SO_REUSEADDR, (char *)&optval, sizeof(optval));
	if(retval == SOCKET_ERROR) err_quit("setsockopt()");*/

	// bind()
	SOCKADDR_IN remoteaddr;
	ZeroMemory(&remoteaddr, sizeof(remoteaddr));
	remoteaddr.sin_family = AF_INET;
	remoteaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	remoteaddr.sin_port = htons(LOCALPORT);
	
	//// 데이터 통신에 사용할 변수
	//SOCKADDR_IN peeraddr;
	//ZeroMemory(&peeraddr, sizeof(peeraddr));
	char username[BUFSIZE];
	char buf[BUFSIZE + 1];
	ZeroMemory(buf, BUFSIZE + 1);
	ZeroMemory(username, BUFSIZE);
	Packet packet;
	ZeroMemory(&packet, sizeof(Packet));
	packet.protocol = PROTOCOL::MAIN;

	retval = sendto(sock, (char*)&packet, sizeof(Packet), 0, (SOCKADDR*)&remoteaddr, sizeof(remoteaddr));
	if (retval == SOCKET_ERROR)
	{
		err_display("sendto1()");
	}
	int addrlen = sizeof(remoteaddr);
	retval = recvfrom(sock, buf, BUFSIZE + 1, 0, (SOCKADDR*)&remoteaddr, &addrlen);
	if (retval == SOCKET_ERROR)
	{
		err_display("recvfrom()");
	}
	printf("%s", buf);
	ZeroMemory(buf, BUFSIZE + 1);
	packet.data = 0;
	while (1)
	{

		scanf("%d", &packet.data);

		if (packet.data < 1 || packet.data>3)
			printf("잘못입력하셨습니다. 다시입력하세요!\n");
		else
		{
			printf("채팅명 입력:");
			scanf("%s", username);
			strcpy(packet.buf, username);
			packet.protocol = PROTOCOL::ADDRESS;
			break;
		}
	}
	//방 선택 
	retval = sendto(sock, (char*)&packet, sizeof(Packet), 0, (SOCKADDR*)&remoteaddr, sizeof(remoteaddr));
	if (retval == SOCKET_ERROR)
	{
		err_display("sendto()");
	}
	SOCKADDR_IN multicastaddr;
	ZeroMemory(&multicastaddr, sizeof(multicastaddr));
	retval = recvfrom(sock, (char*)&multicastaddr, sizeof(multicastaddr), 0, (SOCKADDR*)&remoteaddr, &addrlen);
	if (retval == SOCKET_ERROR)
	{
		err_display("recvfrom()");
	}
	closesocket(sock);
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET)
		err_quit("socket()");
	SOCKADDR_IN myaddr;
	ZeroMemory(&myaddr, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(LOCALPORT);

	retval = bind(sock, (SOCKADDR*)&myaddr, sizeof(myaddr));
	if (retval == SOCKET_ERROR)
		err_display("bind()");

	// 멀티캐스트 그룹 가입
	struct ip_mreq mreq;
	//가입할 주소
	mreq.imr_multiaddr.s_addr = multicastaddr.sin_addr.s_addr;
	//내주소
	mreq.imr_interface.s_addr = inet_addr("127.0.0.1");

	retval = setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
		(char*)&mreq, sizeof(mreq));

	if (retval == SOCKET_ERROR) err_quit("setsockopt()");
	packet.protocol = PROTOCOL::CHAT;
	
	HANDLE hThread = CreateThread(NULL, 0, RecvThread, (LPVOID)sock, 0, NULL);
	SOCKADDR_IN peeraddr;
	memcpy(&peeraddr, &multicastaddr, sizeof(multicastaddr));
	
	// 멀티캐스트 데이터 받기
	while (1)
	{
		printf("[%s]:", username);
		scanf("%s", packet.buf);
		retval = sendto(sock, (char*)&packet, sizeof(packet), 0,NULL, 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("sendto");
		}
	}

	// 멀티캐스트 그룹 탈퇴
	retval = setsockopt(sock, IPPROTO_IP, IP_DROP_MEMBERSHIP,
		(char *)&mreq, sizeof(mreq));
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");

	// closesocket()
	closesocket(sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}