#pragma comment (lib,"ws2_32")
#include <winsock2.h>
#include <stdio.h>

#define BUFSIZE 512
#define SERVERIP "127.1.1.1"
#define SERVERPORT 9000
#define MENUSTRING "메뉴를 선택하세요\n1.박용택방\n2.오지환방\n3.김현수방"
#define NICKNAMEMSG "사용하실 닉네임 입력해주세요\n"

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
void GetProtocol(const char* buf,PROTOCOL* protocol)
{
	const char* ptr = buf;
	memcpy(protocol, buf, sizeof(PROTOCOL));
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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	int retval = 0;
	WSADATA ws;
	if (WSAStartup(MAKEWORD(2, 2), &ws) != 0)
		return -1;
	SOCKET sock=socket(AF_INET,SOCK_STREAM,0);
	SOCKADDR_IN addr;
	ZeroMemory(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(SERVERIP);
	addr.sin_port = htons(SERVERPORT);

	int addrlen=0;
	NetworkPacket packet;
	UserInfo user;
	ZeroMemory(&user, sizeof(user));
	ZeroMemory(&packet, sizeof(NetworkPacket));
	//connect
	retval = connect(sock, (SOCKADDR*)&addr, sizeof(addr));
	if (retval == SOCKET_ERROR)
		err_quit("connect()");

	char buf[BUFSIZE];
	int size = 0;
	char number = 0;
	while (1)
	{
		if (!RecvPacket(sock, packet.Data))
		{
			MessageBox(NULL, (LPCTSTR)"데이터받기 실패\n", (LPCTSTR)"수신실패", MB_OK);
		}
		GetProtocol(packet.Data, &packet.protocol);
		switch (packet.protocol)
		{
		case PROTOCOL::INPUTNICKNAME:
			//입력하시오 출력
			ZeroMemory(buf, BUFSIZE);
			UnPackPacket(packet.Data, buf);
			printf("%s", buf);

			//입력값과 메뉴요청 전송
			ZeroMemory(user.nickname, BUFSIZE);
			scanf("%s", user.nickname);

			size = PackPacking(packet.Data, PROTOCOL::MENU, user.nickname);
			retval = send(sock, packet.Data, size, 0);
			if (retval == SOCKET_ERROR)
				err_display("send()");
				break;
		case PROTOCOL::MENU:
			//받아온 메뉴 출력
			ZeroMemory(buf, BUFSIZE);
			UnPackPacket(packet.Data, buf);
			printf("%s", buf);
			
			//방 선택 및 방 정보 전송
			
			ZeroMemory(packet.Data, BUFSIZE);
			scanf("%c", &number);
			PackPacking(packet.Data, PROTOCOL::ADDR, &number);
			break;
		case PROTOCOL::ADDR:
			break;
		}
	}
	
	closesocket(sock);
	WSACleanup();
	return 0;
}