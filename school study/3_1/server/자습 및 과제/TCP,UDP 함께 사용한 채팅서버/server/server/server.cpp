#pragma comment (lib,"ws2_32")
//#include <Windows.h>
#include <winsock2.h>
#include <stdio.h>
#define BUFSIZE 512
#define SERVERIP "127.1.1.1"
#define SERVERPORT 9000
#define MENUSTRING "�޴��� �����ϼ���\n1.�ڿ��ù�\n2.����ȯ��\n3.��������"
#define NICKNAMEMSG "����Ͻ� �г��� �Է����ּ���\n"

DWORD WINAPI ClientThread(LPVOID arg);

enum class PROTOCOL
{
	NONE = -1,
	INPUTNICKNAME,
	MENU,
	ADDR,
	CHATTING,
	MAX
};
enum class STATE
{
	NONE = -1,
	START,
	MENU,
	ADDR,
	CHATTING,
	MAX
};
typedef struct NetworkPacket
{
	char Data[BUFSIZE];
	PROTOCOL protocol;
};
typedef struct UserInfo
{
	char nickname[BUFSIZE];
	SOCKET sock;
	SOCKADDR_IN addr;
};

void err_quit(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		0, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, (LPCTSTR)msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}
void err_display(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		0, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, lpMsgBuf);
	LocalFree(lpMsgBuf);
}
int recvn(SOCKET sock, char* buf, int len, bool flag)
{
	char* ptr = buf;
	int left = len;
	int recved = 0;
	while (left > 0)
	{
		recved = recv(sock, ptr, left, flag);
		if (recved == SOCKET_ERROR)
			return SOCKET_ERROR;
		if (recved == 0)
			break;
		left -= recved;
		ptr += recved;

	}
	return (len - left);
}
bool RecvPacket(SOCKET s, char* recvbuf)
{
	int size = 0;
	int retval = recvn(s, (char*)&size, sizeof(int), 0);
	if (retval == SOCKET_ERROR)
		return false;
	else if (retval == 0)
		return 0;
	retval = recvn(s, recvbuf, size, 0);
	if (retval == SOCKET_ERROR)return false;
	else if (retval == 0) return false;

	return true;
}
int PackPacking(char* buf, PROTOCOL protocol, const char* msg)
{
	char* ptr = buf + sizeof(int);
	int strsize = strlen(msg);
	int size = 0;
	memcpy(ptr, &protocol, sizeof(PROTOCOL));
	size += sizeof(PROTOCOL);
	ptr += sizeof(PROTOCOL);
	if (msg != NULL)
	{
		memcpy(ptr, &strsize, sizeof(int));
		ptr += sizeof(int);
		size += sizeof(int);

		memcpy(ptr, msg, strsize);
		ptr = buf;
		size += strsize;
	}
	memcpy(ptr, &size, sizeof(int));
	size += sizeof(int);
	return size;
}
void UnPackPacket(const char* buf, char* nickname)
{
	const char* ptr = buf+sizeof(PROTOCOL);
	int strsize = 0;
	memcpy(&strsize, ptr, sizeof(int));
	ptr += sizeof(int);
	memcpy(nickname, ptr, strsize);
}
int main(int argv, char* argc[])
{
	int retval = 0;
	WSADATA ws;
	if (WSAStartup(MAKEWORD(2, 2), &ws) != 0)
		return -1;

	//���ϻ��� �� ���ε�
	/*������ ������ �Ѵٰ� ����� ���� �ʴ´�.
	������ �ϴ� ���� bind�� �ؼ� Ŀ�ο� socket�� �÷����� ������ �����ϴ�.*/
	SOCKET listensock = socket(AF_INET, SOCK_STREAM, 0);
	if (listensock == INVALID_SOCKET)
		err_quit("socket()");
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listensock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		err_quit("bind()");
	retval = listen(listensock, SOMAXCONN);
	if (retval == SOCKET_ERROR)
		err_quit("listen()");

	SOCKET sock;
	int addrlen = sizeof(serveraddr);
	while (1)
	{
		//accept
		sock = accept(listensock, (SOCKADDR*)&serveraddr, &addrlen);
		if (sock == INVALID_SOCKET)
			err_quit("accept");

		printf("[TCP/IP ����] IP: %s PORT: %d", inet_ntoa(serveraddr.sin_addr), ntohs(serveraddr.sin_port));
		UserInfo user;
		ZeroMemory(&user, sizeof(user));
		user.sock = sock;
		memcpy(&user.addr, &serveraddr, sizeof(serveraddr));

		//������ ����
		HANDLE hThread = CreateThread(0, NULL, ClientThread, (LPVOID)&user, NULL, 0);
		if (hThread != NULL)
			CloseHandle(hThread);

	}

	WSACleanup();
	return 0;
}
DWORD WINAPI ClientThread(LPVOID arg)
{
	UserInfo user = *(UserInfo*)arg;
	NetworkPacket packet;
	ZeroMemory(&packet, sizeof(packet));
	STATE state = STATE::START;
	int size = 0;
	int retval = 0;
	switch (state)
	{
	case STATE::START:
		// �г��� �Է��Ͻÿ� ����.
		size = PackPacking(packet.Data, PROTOCOL::INPUTNICKNAME, NICKNAMEMSG);
		retval = send(user.sock, packet.Data, size, 0);
		if (retval == SOCKET_ERROR)
			err_display("send()");
		state = STATE::MENU;
		break;
	case STATE::MENU:
		//�г��� �޾ƿ��� �޴� ����.
		ZeroMemory(packet.Data, BUFSIZE);
		if (!RecvPacket(user.sock, packet.Data))
		{
			printf("������ �ޱ� ����");
			return 0;
		}
		printf("[%s]�� ����");
		ZeroMemory(packet.Data, BUFSIZE);
		size = PackPacking(packet.Data, PROTOCOL::MENU, MENUSTRING);
		retval = send(user.sock, packet.Data, size, 0);
		if (retval == SOCKET_ERROR)
			err_display("send()");
		state=STATE::ADDR;
		break;
	case STATE::ADDR:

		break;

	}

	


}