#pragma comment (lib,"ws2_32")
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#define BUFSIZE 512
#define SERVERIP "127.0.0.1"
#define PORT 9000

ip_mreq mreq;
//���� �÷���
bool flag = false;
enum class PROTOCOL
{
	NONE = 0,
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
		if (flag)
		{
			printf("������ ��������");
			break;
		}
		ZeroMemory(buf, BUFSIZE);
		peerlen = sizeof(multipeeraddr);
		retval = recvfrom(sock, (char*)&packet,sizeof(packet), 0,(SOCKADDR*)&multipeeraddr, &peerlen);
		if (retval == SOCKET_ERROR)
			err_display((char*)"recvfrom2()");
		strcpy(buf, packet.buf);
		printf("[%s]%s\n",packet.username, buf);
	}
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
	SOCKADDR_IN localaddr;
	ZeroMemory(&localaddr, sizeof(localaddr));
	localaddr.sin_family = AF_INET;
	localaddr.sin_addr.s_addr = inet_addr(SERVERIP);
	localaddr.sin_port = htons(PORT);

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
	char buf[BUFSIZE];
	ZeroMemory(buf, BUFSIZE);


	Packet packet;
	packet.protocol = PROTOCOL::MENU;
	int room_number = 0;
	//packet�� ��� ������ localaddr(���ּ�������)�� ����+�ڵ����ε�.
	retval = sendto(sock, (char*)&packet, sizeof(packet), 0, (SOCKADDR*)&localaddr, sizeof(localaddr));
	if (retval == SOCKET_ERROR)
		err_display((char*)"sendto()");
	//�������� �޴������� �޾ƿ�,�����ּ������� peeraddr�� �޾ƿ�.recvfrom()
	retval = recvfrom(sock, (char*)&packet, sizeof(packet), 0, (SOCKADDR*)&peeraddr, &peerlen);
	if (retval == SOCKET_ERROR)
		err_display((char*)"recvfrom");
	if (sizeof(peeraddr) != peerlen)
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
	retval = sendto(sock, (char*)&packet, sizeof(packet), 0, (SOCKADDR*)&localaddr, sizeof(localaddr));
	if (retval == SOCKET_ERROR)
		err_display((char*)"sendto()");
	//�ּ� �޾ƿ�
	retval = recvfrom(sock, (char*)&packet, sizeof(packet), 0, (SOCKADDR*)&peeraddr, &peerlen);
	if (retval == SOCKET_ERROR)
		err_display((char*)"recvfrom1()");
	//������ ���� sock ���ֱ�.
	closesocket(sock);
	sock = INVALID_SOCKET;
	//��Ƽĳ��Ʈ���� ����� ���� ����.
	SOCKET recv_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (recv_sock == INVALID_SOCKET) err_display((char*)"socket()");

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
	multiaddr.sin_port = htons(packet.number);
	retval = bind(recv_sock, (SOCKADDR*)&multiaddr, sizeof(multiaddr));
	if (retval == SOCKET_ERROR)
		err_display((char*)"bind()");

	//send�� ���� �ּұ���ü
	SOCKADDR_IN remoteaddr;
	ZeroMemory(&remoteaddr, sizeof(remoteaddr));
	remoteaddr.sin_family = AF_INET;
	remoteaddr.sin_addr.s_addr = inet_addr(packet.buf);
	remoteaddr.sin_port = htons(packet.number);

	//��Ƽ������ �����
	HANDLE hThread = CreateThread(NULL, 0, RecvThread, (LPVOID)recv_sock, 0, NULL);

	packet.protocol = PROTOCOL::INCHATTING;
	
	//ä��
	while (1)
	{
		ZeroMemory(packet.buf, BUFSIZE);
		
		//scanf("%s", packet.buf);
		if (fgets(packet.buf, BUFSIZE, stdin) == nullptr) 
		{
			return 0;
		}
		//end�Է½� ���α׷� ����
		if (!strcmp(packet.buf, "end\n"))
		{
			flag = true;
			printf("���α׷� ����\n");
			break;
		}
		//������ �Է��� ä�� ���� ����
		retval = sendto(recv_sock, (char*)&packet, sizeof(packet), 0, (SOCKADDR*)&remoteaddr, sizeof(remoteaddr));
		if (retval == SOCKET_ERROR)
			err_display((char*)"sendtooo()");
	}
	//��� ����
	retval = setsockopt(recv_sock, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*)&mreq, sizeof(mreq));
	if (retval == SOCKET_ERROR)err_quit((char*)"setsockopt()");
	//���Ϲݳ�
	closesocket(recv_sock);


	//���� �������
	WSACleanup();
	return 0;
}
/* �ʹ� ������� �Ф� 70�۱����� ȥ���ߴµ� �� �ں��� �𸣰ڰ� ��ư� ���������� ���ã�ƺ���
�ֺ���������� ����� �ؼ� Ǯ�� Ǯ���µ� ��Ȯ�� �� �׷����� �����ϰ� Ǭ�� �ƴѰͰ��Ƽ� �ʹ� �����׿� �Ф�*/