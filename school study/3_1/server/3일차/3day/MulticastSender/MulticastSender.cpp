#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#define MULTICASTIP "235.7.8.9"
#define REMOTEPORT  9000
#define BUFSIZE     512

// ���� �Լ� ���� ��� �� ����
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

// ���� �Լ� ���� ���
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
	// ���� �ʱ�ȭ
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock == INVALID_SOCKET) err_quit("socket()");
	
	// ���� �ּ� ����ü �ʱ�ȭ
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
	// ������ ��ſ� ����� ����
	char buf[BUFSIZE+1];
	int len=sizeof(peeraddr);
	//�渶�� �������� ip�ּ� ����
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
	// ��Ƽĳ��Ʈ ������ ������
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
			strcpy(buf, "�漱��\n1.�ڿ���\n2.����ȯ\n3.������\n");
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
			printf("[UDP IP:%s|PORT:%d] %s ���� %d���� ����\n",inet_ntoa(peeraddr.sin_addr),ntohs(peeraddr.sin_port),username, packet.data);
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
		//// ������ �Է�
		//printf("\n[���� ������] ");
		//if(fgets(buf, BUFSIZE+1, stdin) == NULL)
		//	break;

		//// '\n' ���� ����
		//len = strlen(buf);
		//if(buf[len-1] == '\n')
		//	buf[len-1] = '\0';
		//if(strlen(buf) == 0)
		//	break;

		//// ������ ������
		//retval = sendto(sock, buf, strlen(buf), 0,
		//	(SOCKADDR *)&remoteaddr, sizeof(remoteaddr));
		//if(retval == SOCKET_ERROR)
		//{
		//	err_display("sendto()");
		//	continue;
		//}
		//printf("[UDP] %d����Ʈ�� ���½��ϴ�.\n", retval);
	}

	// closesocket()
	closesocket(sock);

	// ���� ����
	WSACleanup();
	return 0;
}