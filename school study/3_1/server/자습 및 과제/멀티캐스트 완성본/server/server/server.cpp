#pragma comment (lib,"ws2_32")
#include <WinSock2.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFSIZE 512
#define SERVERIP "127.0.0.1"
#define PORT 9000

enum class PROTOCOL
{
	NONE=0,
	MENU,
	ADDRESS,
	INCHATTING,
	MAX,
};
struct Packet
{
	Packet() 
	{ 
		ZeroMemory(buf, BUFSIZE);
		ZeroMemory(username, BUFSIZE);
	}
	PROTOCOL protocol;
	char username[BUFSIZE];
	char buf[BUFSIZE];
	int number;
};
struct Address
{
	Address(const char* addr)
	{
		ZeroMemory(buf, BUFSIZE);
		strcpy(buf, addr);
	}
	char buf[BUFSIZE];
};
void err_quit(char* msg)
{
	 LPVOID lpMsgBuf;
	 FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL,
		 GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	 MessageBox(NULL, (LPCTSTR)lpMsgBuf, (LPCWSTR)msg, MB_ICONERROR);
	 LocalFree(lpMsgBuf);
	 exit(1);	
}
void err_display(char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL,
		GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

int main(int argv,char* argc[])
{
	int retval = 0;
	//��Ƽĳ��Ʈ���� ����� �ּҿ� ��Ʈ��ȣ�� ������ �ֽ��ϴ�.
	Address addr[3] = { ("224.0.0.0"),("224.0.0.1"),("224.0.0.2") };
	int port[3] = { 9010,9020,9030 };

	WSADATA ws;
	if (WSAStartup(MAKEWORD(2, 2), &ws) != 0)
		return 1;
	//socket
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET)
		err_quit((char*)"socket()");
	SOCKADDR_IN localaddr;
	ZeroMemory(&localaddr, sizeof(localaddr));
	localaddr.sin_family = AF_INET;
	localaddr.sin_addr.s_addr= inet_addr(SERVERIP);
	localaddr.sin_port = htons(PORT);

	//�����ǿ� ��Ʈ��ȣ�� addrlist��� �迭�� �־�ΰ� ����մϴ�.
	SOCKADDR_IN addrlist[3];
	for (int i = 0; i < 3; i++)
	{
		addrlist[i].sin_addr.s_addr = inet_addr((const char*)&addr[i]);
		addrlist[i].sin_port = htons(port[i]);
	}

	int locallen=0;
	//bind
	retval = bind(sock, (SOCKADDR*)&localaddr, sizeof(localaddr));
	if (retval == SOCKET_ERROR)
		err_quit((char*)"bind()");
	//�޾ƿ� ��� �ּ� ���� ����.
	SOCKADDR_IN peeraddr;
	ZeroMemory(&peeraddr, sizeof(peeraddr));
	int peerlen = sizeof(peeraddr);
	char buf[BUFSIZE];
	ZeroMemory(buf, BUFSIZE);
	Packet packet;
	while (1)
	{
		ZeroMemory(&packet, sizeof(packet));
		//Ŭ�󿡰Լ� ������ �޾ƿ��� Ŭ���ּҸ� peeraddr�� ����.
		retval = recvfrom(sock, (char*)&packet, sizeof(packet), 0, (SOCKADDR*)&peeraddr, &peerlen);
		if (retval == SOCKET_ERROR)
			err_display((char*)"recvfrom");
		//protocol�� ���� �����ϴ� ����� �ٸ�.
		switch (packet.protocol)
		{   //�޴� ��û�϶�
			case PROTOCOL::MENU:
			sprintf(packet.buf, "%s", "1.�ڿ���\n2.����ȯ\n3.������\n�Է�:");
			//�޴� ������ Ŭ�󿡰� �����Ѵ�. sendto()
			retval = sendto(sock, (char*)&packet, sizeof(packet), 0, (SOCKADDR*)&peeraddr, sizeof(peeraddr));
			if (retval == SOCKET_ERROR)
				err_display((char*)"sendto()");
			printf("[UDP] %d ����Ʈ ���½��ϴ�.\n", retval);
			break;
			//�ּ� ��û�϶�
			case PROTOCOL::ADDRESS:
				//Ŭ�󿡼� �Է��� �� ��ȣ�� ���� �������� ����濡 �����ߴ��� ������ ����ϰ�
				// ��Ƽĳ��Ʈ���� ����� ip�� port��ȣ�� �����Ѵ�.
				switch (packet.number)
				{
				case 1:
					printf("�ڿ��ù� %s�� ����\n",packet.username);
					//ip����
					strcpy(packet.buf, addr[0].buf);
					break;
				case 2:
					printf("����ȯ�� %s�� ����\n", packet.username);
					//ip����
					strcpy(packet.buf, addr[1].buf);
					break;
				case 3:
					printf("�������� %s�� ����\n", packet.username);
					//ip����
					strcpy(packet.buf, addr[2].buf);
					break;
				}
				//port��ȣ����
				packet.number = port[packet.number - 1];
				retval = sendto(sock, (char*)&packet, sizeof(packet), 0, (SOCKADDR*)&peeraddr, sizeof(peeraddr));
				if (retval == SOCKET_ERROR)
				{
					err_display((char*)"sendto()");
			    }
				break;
				//ä�����϶�
			case PROTOCOL::INCHATTING:
				//packet�� ��� buf(ä��),username(�����̸�)�� �����Ѵ�.
				retval = sendto(sock, (char*)&packet, sizeof(packet), 0, (SOCKADDR*)&peeraddr, sizeof(peeraddr));
				if (retval == SOCKET_ERROR)
				{
					err_display((char*)"sendto()");
				}
				break;
		}
	}
	
	//recvfrom()
	//closesocket()
	closesocket(sock);
	WSACleanup();
	return 0;
}