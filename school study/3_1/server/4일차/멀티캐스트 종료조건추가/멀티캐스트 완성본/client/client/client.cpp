#pragma comment (lib,"ws2_32")
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#define BUFSIZE 512
#define SERVERIP "127.0.0.1"
#define PORT 9000

enum class PROTOCOL
{
	NONE = 0,
	MENU,
	ADDRESS,
	INCHATTING,
	END,
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
DWORD WINAPI RecvThread(LPVOID lpParam)
{
	SOCKET sock = (SOCKET)lpParam;
	SOCKADDR_IN multipeeraddr;
	ZeroMemory(&multipeeraddr, sizeof(multipeeraddr));
	int peerlen = 0;
	char buf[BUFSIZE];
	int retval = 0;
	Packet packet;

	if (retval == SOCKET_ERROR) err_quit((char*)"sockopt()");
	while (1)
	{   //����������
		ZeroMemory(buf, BUFSIZE);
		peerlen = sizeof(multipeeraddr);
		retval = recvfrom(sock, (char*)&packet, sizeof(packet), 0, (SOCKADDR*)&multipeeraddr, &peerlen);
		if (retval == SOCKET_ERROR)
			err_display((char*)"recvfrom2()");
		//end�Է½� ���α׷� ����
		if (packet.protocol == PROTOCOL::END)
		{
			char* ptr = packet.buf;
			SOCKET tempsock;
			//�����û�� Ŭ�� �����ϱ� ���ؼ� ���� buf�� sock�� ����
			memcpy(&tempsock, ptr, sizeof(SOCKET));
			ptr += sizeof(SOCKET);
			int strsize = 0;
			//���ڿ� ������ �ޱ�
			memcpy(&strsize, ptr, sizeof(int));
			ptr += sizeof(int);
			//���ڿ� ����
			memcpy(buf, ptr, strsize);
			//�Է°��� end�̸� ������ �����û�� ���ϰ� �����ϴٸ� ����
			if (!strcmp(buf, "end\n")&&sock==tempsock)
			{
				break;
			}//�����û�� Ŭ�� �ƴҰ��
			else if (!strcmp(buf,"end\n"))
			{
					sprintf(buf, "%s���� �����ϼ̽��ϴ�.\n", packet.username);
					printf("%s", buf);
			}
		}
		else
		{     //�����û�� �ƴ� �Ϲ� ä���� ���
				strcpy(buf, packet.buf);
				printf("\n[%s]%s\n", packet.username, buf);
		}
	}
	printf("������ ���α׷� ����\n");
	return 0;
}
int main(int argv, char* argc[])
{
	int retval = 0;
	WSADATA ws;
	if (WSAStartup(MAKEWORD(2, 2), &ws) != 0)
		return 1;
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET)
		err_quit((char*)"socket()");

	//sand�Ҷ� ����ϱ� ���� �̰��� �ּ�
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(PORT);
	int serverlen = sizeof(serveraddr);
	//������ �����ص� �ٷ� �Ҹ��ϴ°��� �ƴ϶� �� ������ �����Ҷ� ���� ���� �����ϸ� Ŀ�ο��� ����ó���ϴµ�
	//�װ��� SO_REUSEADDR�� ���� ����Ͽ� ������ �ȳ��� ������.
	BOOL bEnable = TRUE;
	retval = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
		(char*)&bEnable
		, sizeof(bEnable));
	//����ּ� ���� ����ü
	SOCKADDR_IN peeraddr;
	ZeroMemory(&peeraddr, sizeof(peeraddr));
	int peerlen = sizeof(peeraddr);
	


	Packet packet;
	packet.protocol = PROTOCOL::MENU;
	int room_number = 0;
	//packet�� ��� ������ serveraddr(���ּ�������)�� ����+�ڵ����ε�.
	retval = sendto(sock, (char*)&packet, sizeof(packet), 0, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		err_display((char*)"sendto()");
	//�������� �޴������� �޾ƿ�,�����ּ������� peeraddr�� �޾ƿ�.recvfrom()
	retval = recvfrom(sock, (char*)&packet, sizeof(packet), 0, (SOCKADDR*)&serveraddr, &serverlen);
	if (retval == SOCKET_ERROR)
		err_display((char*)"recvfrom");
	if (sizeof(serveraddr) != serverlen)
	{
		printf("�߸��� ����\n");
	}
	//�޴��� ���� ������ ���� �޾ƿ�.
	while (1)
	{
		printf("%s\n", packet.buf);
		ZeroMemory(packet.buf, BUFSIZE);
		scanf("%d", &packet.number);
		//1~3���� �ƴ� �� �Է½� �ٽ� �Է��ϵ��� ó��
		if (packet.number > 3 || packet.number <= 0)
			printf("�ٽ��Է��ϼ���!\n");
		else
		{
			//�����̸��ޱ�
			printf("�����̸��Է�:");
			scanf("%s", packet.username);
			getchar();
			break;
		}
	}
	packet.protocol = PROTOCOL::ADDRESS;
	//�ּ� ��û
	retval = sendto(sock, (char*)&packet, sizeof(packet), 0, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		err_display((char*)"sendto()");
	//�ּ� �޾ƿ�
	retval = recvfrom(sock, (char*)&packet, sizeof(packet), 0, (SOCKADDR*)&serveraddr, &serverlen);
	if (retval == SOCKET_ERROR)
		err_display((char*)"recvfrom1()");
	//������ ���� sock ���ֱ�.
	closesocket(sock);
	sock = INVALID_SOCKET;
	//��Ƽĳ��Ʈ���� ����� ���� ����.
	SOCKET recv_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (recv_sock == INVALID_SOCKET) err_display((char*)"socket()");
	ip_mreq mreq;
	//�������ּ�= �������� �޾ƿ� ��Ƽĳ��Ʈ �ּ�
	mreq.imr_multiaddr.s_addr = inet_addr(packet.buf);
	//���ּ�= �ƹ��ּ�
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	//��Ƽĳ��Ʈ ������
	retval = setsockopt(recv_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq));
	if (retval == SOCKET_ERROR)err_quit((char*)"setsockopt()");

	//recv_sock�� ���� REUSE ����.
	bEnable = TRUE;
	retval = setsockopt(recv_sock, SOL_SOCKET, SO_REUSEADDR, (char*)&bEnable, sizeof(bEnable));

	//recv�ޱ� ���� ���ε� �۾�.
	SOCKADDR_IN multiaddr;
	ZeroMemory(&multiaddr, sizeof(multiaddr));
	multiaddr.sin_family = AF_INET;
	//�ƹ� �����ǳ� ����.
	multiaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	//�������� �޾ƿ� �濡���� ��Ʈ��ȣ
	multiaddr.sin_port = htons(9000);//packet.number);//������Ʈ��ȣ�� �����ѵ� REUSE������ �ؾߵɵ�?
	retval = bind(recv_sock, (SOCKADDR*)&multiaddr, sizeof(multiaddr));
	if (retval == SOCKET_ERROR)
		err_display((char*)"bind()");

	//send�� ���� �ּұ���ü
	SOCKADDR_IN remoteaddr;
	ZeroMemory(&remoteaddr, sizeof(remoteaddr));
	remoteaddr.sin_family = AF_INET;
	remoteaddr.sin_addr.s_addr = inet_addr(packet.buf);
	remoteaddr.sin_port = htons(9000);//packet.number);//������Ʈ��ȣ�� �����ѵ� REUSE������ �ؾߵɵ�?

	//��Ƽ������ �����
	HANDLE hThread = CreateThread(NULL, 0, RecvThread, (LPVOID)recv_sock, 0, NULL);

	char buf[BUFSIZE];
	
	//ä��
	while (1)
	{
		packet.protocol = PROTOCOL::INCHATTING;
		ZeroMemory(buf, BUFSIZE);
		ZeroMemory(packet.buf, BUFSIZE);

		//scanf("%s", packet.buf);
		if (fgets(buf, BUFSIZE, stdin) == nullptr)
		{
			return 0;
		}
		if (!strcmp(buf, "end\n"))
		{
			//�����û�Ͻ� ���ۿ� ���� ������ ����
			packet.protocol = PROTOCOL::END;
			char* ptr = packet.buf;
			int strsize = strlen(buf);
			memcpy(ptr, &recv_sock, sizeof(recv_sock));
			ptr += sizeof(recv_sock);
			memcpy(ptr,&strsize,sizeof(int));
			ptr += sizeof(int);
			memcpy(ptr, buf, strsize);
		}
		else
		{ //ä���Ͻ� ���ڿ� ����
			strcpy(packet.buf, buf);
		}
		//������ �Է��� ä�� ���� ����
		retval = sendto(recv_sock, (char*)&packet, sizeof(packet), 0, (SOCKADDR*)&remoteaddr, sizeof(remoteaddr));
		if (retval == SOCKET_ERROR)
			err_display((char*)"sendtooo()");
		if (!strcmp(buf, "end\n"))
		{
			//������ �Լ��� ����Ǹ� wait �Լ��� ���ϵǾ� wait���� Ż��
			WaitForSingleObject(hThread, INFINITE);
			break;
		}
	}
	//��� ����
	retval = setsockopt(recv_sock, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*)&mreq, sizeof(mreq));
	if (retval == SOCKET_ERROR)err_quit((char*)"setsockopt()");
	//���Ϲݳ�
	closesocket(recv_sock);

	//���� �������
	WSACleanup();
	printf("���� ��������\n");
	return 0;
}
/* �ʹ� ������� �Ф� 70�۱����� ȥ���ߴµ� �� �ں��� �𸣰ڰ� ��ư� ���������� ���ã�ƺ���
�ֺ���������� ����� �ؼ� Ǯ�� Ǯ���µ� ��Ȯ�� �� �׷����� �����ϰ� Ǭ�� �ƴѰͰ��Ƽ� �ʹ� �����׿� �Ф�*/