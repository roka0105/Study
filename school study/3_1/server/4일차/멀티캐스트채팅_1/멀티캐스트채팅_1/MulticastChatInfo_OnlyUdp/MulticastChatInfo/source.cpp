#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#define BUFSIZE 512

enum PROTOCOL 
{ 
	CHATINFOREQ = 1, ADDRREQ 
};

struct _RequestPacket
{
	PROTOCOL protocol;
	int Data;
}RequestPacket;



// ���� �Լ� ���� ��� �� ����
void err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(-1);
}

// ���� �Լ� ���� ���
void err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (LPCTSTR)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

int main(int argc, char* argv[])
{
	int retval;
	char msg[] = "ä�ù��� ������!!! \r\n1.�ڿ��ù�\r\n2.����ȯ��\r\n3.��������\r\n";

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return -1;

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");


	// bind()
	SOCKADDR_IN localaddr;
	ZeroMemory(&localaddr, sizeof(localaddr));
	localaddr.sin_family = AF_INET;
	localaddr.sin_port = htons(9900);
	localaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	retval = bind(sock, (SOCKADDR *)&localaddr, sizeof(localaddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");


	SOCKADDR_IN chatR_1, chatR_2, chatR_3;
	ZeroMemory(&chatR_1, sizeof(chatR_1));
	chatR_1.sin_family = AF_INET;
	chatR_1.sin_port = htons(9001);
	chatR_1.sin_addr.s_addr = inet_addr("235.7.8.9");

	ZeroMemory(&chatR_2, sizeof(chatR_2));
	chatR_2.sin_family = AF_INET;
	chatR_2.sin_port = htons(9002);
	chatR_2.sin_addr.s_addr = inet_addr("235.7.8.10");

	ZeroMemory(&chatR_3, sizeof(chatR_3));
	chatR_3.sin_family = AF_INET;
	chatR_3.sin_port = htons(9003);
	chatR_3.sin_addr.s_addr = inet_addr("235.7.8.11");




	// ������ ��ſ� ����� ����
	SOCKADDR_IN peeraddr, multiaddr;
	int addrlen;
	char buf[BUFSIZE + 1];

	// ��Ƽĳ��Ʈ ������ �ޱ�
	while (1)
	{
		// ������ �ޱ�
		addrlen = sizeof(peeraddr);
		retval = recvfrom(sock, (char*)&RequestPacket, sizeof(RequestPacket), 0,
			(SOCKADDR *)&peeraddr, &addrlen);
		if (retval == SOCKET_ERROR) {
			err_display("recvfrom()");
			continue;
		}

		switch (RequestPacket.protocol)
		{
		case CHATINFOREQ:
			retval = sendto(sock, msg, strlen(msg), 0, (SOCKADDR*)&peeraddr, sizeof(peeraddr));
			if (retval == SOCKET_ERROR)
			{
				err_display("recvfrom()");
				continue;
			}

			break;
		case ADDRREQ:
			switch (RequestPacket.Data)
			{
			case 1:
				memcpy(&multiaddr, &chatR_1, sizeof(SOCKADDR));
				break;
			case 2:
				memcpy(&multiaddr, &chatR_2, sizeof(SOCKADDR));
				break;
			case 3:
				memcpy(&multiaddr, &chatR_3, sizeof(SOCKADDR));
				break;
			}

			retval = sendto(sock, (char*)&multiaddr, sizeof(SOCKADDR), 0, (SOCKADDR*)&peeraddr, sizeof(peeraddr));
			if (retval == SOCKET_ERROR)
			{
				err_display("recvfrom()");
				continue;
			}
			buf[retval] = '\0';
			printf("[UDP/%s:%d] %d���� ����\n", inet_ntoa(peeraddr.sin_addr),
				ntohs(peeraddr.sin_port), RequestPacket.Data);
			break;
		}	
	}
	// closesocket()
	closesocket(sock);

	// ���� ����
	WSACleanup();
	return 0;
}