#pragma comment (lib,"ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERPORT 9000
#define BUFSIZE 512

void err_quit(char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, (LPCWSTR)msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}
void err_display(char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

int main(int argv, char* argc[])
{

	int retval;
	WSADATA ws;
	if (WSAStartup(MAKEWORD(2, 2), &ws) != 0)
		return 1;
	// socket()
	SOCKET sock;
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET)
		err_quit((char*)"socket()");

	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	// bind()
	retval = bind(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		err_display((char*)"bind()");
	char buf[BUFSIZE];
	ZeroMemory(buf, BUFSIZE);

	SOCKADDR_IN clientaddr;
	int clientaddrlen = 0;
	//while(1)
	//{
		ZeroMemory(&clientaddr, sizeof(clientaddr));
		clientaddrlen = sizeof(clientaddr);
		// recvfrom()
		retval = recvfrom(sock, buf, BUFSIZE, 0, (SOCKADDR*)&clientaddr, &clientaddrlen);
		if (retval == SOCKET_ERROR)
			err_display((char*)"recvfrom()");

		printf("[UDP IP:%s PORT:%d]받아온 데이터 : %s\n",inet_ntoa(serveraddr.sin_addr)
			,ntohs(serveraddr.sin_port), buf);

		ZeroMemory(buf, BUFSIZE);
		printf("입력하세요:");
		scanf("%s", buf);

		int strsize = strlen(buf);
		// sendto()
		retval = sendto(sock, buf, strsize, 0, (SOCKADDR*)&clientaddr, sizeof(clientaddr));
		if (retval == SOCKET_ERROR)
			err_display((char*)"sendto()");

		printf("데이터 %d 바이트 전송\n", strsize);

	//}
	
    // closesocket()
	closesocket(sock);
	WSACleanup();
 	return 0;
}
