#pragma comment (lib,"ws2_32")
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFSIZE 512
#define PORT 9000
#define SERVERIP "127.0.0.1"

void err_quit(char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, 0,
		WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf,
		0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, (LPCWSTR)msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}
void err_display(char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, 0,
		WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf,
		0, NULL);
	printf("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

int main(int argv, char* argc[])
{
	int retval = 0;
	WSADATA ws;
	if (WSAStartup(MAKEWORD(2, 2), &ws) != 0)
		return 1;
	SOCKET sock;
	// socket()
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET)
		err_quit((char*)"socket()");

	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(PORT);

	int serverlen = 0;
	char buf[BUFSIZE];
	
	//while (1)
	//{
		ZeroMemory(buf, BUFSIZE);
		serverlen = sizeof(serveraddr);
		printf("입력하세요:");
		scanf("%s", buf);
		// sendto()
		retval = sendto(sock, buf, strlen(buf), 0, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
		if (retval == SOCKET_ERROR)
			err_display((char*)"sendto()");
		printf("데이터 %d 바이트 전송\n", strlen(buf));

		ZeroMemory(buf, BUFSIZE);
		SOCKADDR_IN peeraddr;
		// recvfrom()
		retval = recvfrom(sock, buf, BUFSIZE, 0, (SOCKADDR*)&peeraddr, &serverlen);
		if (retval == SOCKET_ERROR)
			err_display((char*)"recvfrom()");

		if (memcmp(&serveraddr, &peeraddr, sizeof(peeraddr)))
		{
			printf("주소가 일치하지 않습니다.\n");
		}
		printf("%s", buf);
	//}

	// closesocket()
	closesocket(sock);
	WSACleanup();
	return 0;
}
