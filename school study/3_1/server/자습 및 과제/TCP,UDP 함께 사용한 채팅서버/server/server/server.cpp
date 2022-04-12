#pragma comment (lib,"ws2_32")
//#include <Windows.h>
#include <winsock2.h>
#include <stdio.h>
#define BUFSIZE 512
#define SERVERIP "127.1.1.1"
#define SERVERPORT 9000
#define MENUSTRING "메뉴를 선택하세요\r\n1.박용택방\r\n2.오지환방\r\n3.김현수방"
#define NICKNAMEMSG "사용하실 닉네임 입력해주세요\n"

DWORD WINAPI ClientThread(LPVOID arg);

enum class PROTOCOL
{
	NONE = -1,
	INPUTNICKNAME,
	MENU,
	ADDR,
	CHATTING,
	END,
	MAX
};
enum class STATE
{
	NONE = -1,
	START,
	MENU,
	ADDR,
	CHATTING,
	END,
	MAX
};
typedef struct NetworkPacket
{
	char Data[BUFSIZE];
	PROTOCOL protocol;
};
typedef struct UdpPacket
{
	PROTOCOL protocol;
	char name[BUFSIZE];
	char chat[BUFSIZE];
};
typedef struct UserInfo
{
	UserInfo()
	{
		ZeroMemory(nickname, BUFSIZE);
		ZeroMemory(&addr, sizeof(addr));
	}
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
	int strsize = 0; 
	int size = 0;
	memcpy(ptr, &protocol, sizeof(PROTOCOL));
	size += sizeof(PROTOCOL);
	ptr += sizeof(PROTOCOL);
	if (msg != NULL)
	{
		strsize= strlen(msg);
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
int PackPacking(char* buf, PROTOCOL protocol, const char* msg, int* num)
{
	char* ptr = buf + sizeof(int);
	int strsize = strlen(msg);
	int size = 0;
	memcpy(ptr, &protocol, sizeof(PROTOCOL));
	size += sizeof(PROTOCOL);
	ptr += sizeof(PROTOCOL);
	memcpy(ptr, num, sizeof(int));
	size += sizeof(int);
	ptr += sizeof(int);
	if (msg != NULL)
	{
		memcpy(ptr, &strsize, sizeof(int));
		ptr += sizeof(int);
		size += sizeof(int);

		memcpy(ptr, msg, strsize);
		size += strsize;
	}
	ptr = buf;
	memcpy(ptr, &size, sizeof(int));
	size += sizeof(int);

	return size;
}
void UnPackPacket(const char* buf, char* msg)
{
	const char* ptr = buf + sizeof(PROTOCOL);
	int strsize = 0;
	memcpy(&strsize, ptr, sizeof(int));
	ptr += sizeof(int);
	memcpy(msg, ptr, strsize);
}
void GetProtocol(const char* buf, PROTOCOL* protocol)
{
	const char* ptr = buf;
	memcpy(protocol, ptr, sizeof(PROTOCOL));
}
int main(int argv, char* argc[])
{
	int retval = 0;
	WSADATA ws;
	if (WSAStartup(MAKEWORD(2, 2), &ws) != 0)
		return -1;

	//소켓생성 및 바인딩
	/*소켓은 생성만 한다고 통신이 되지 않는다.
	수신을 하는 쪽은 bind를 해서 커널에 socket을 올려놔야 수신이 가능하다.*/
	SOCKET listensock = socket(AF_INET, SOCK_STREAM, 0);
	if (listensock == INVALID_SOCKET)
		err_quit("socket()");
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
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

		printf("[TCP/IP 접속] IP: %s PORT: %d\n", inet_ntoa(serveraddr.sin_addr), ntohs(serveraddr.sin_port));
		UserInfo* user = new UserInfo();
		ZeroMemory(user, sizeof(UserInfo));
		user->sock = sock;
		memcpy(&user->addr, &serveraddr, sizeof(serveraddr));

		//스레드 생성
		HANDLE hThread = CreateThread(0, NULL, ClientThread, (LPVOID)user, NULL, 0);
		if (hThread != NULL)
			CloseHandle(hThread);

	}

	WSACleanup();
	return 0;
}
DWORD WINAPI ClientThread(LPVOID arg)
{
	UserInfo* user = (UserInfo*)arg;
	UserInfo udp;
	NetworkPacket packet;
	ZeroMemory(&packet, sizeof(packet));
	STATE state = STATE::START;
	int size = 0;
	int retval = 0;
	char buf[BUFSIZE];
	int menunumber = 0;

	char IpArray[3][BUFSIZE] = { "224.1.1.1","224.1.1.2","224.1.1.3" };
	int PortArray[3] = { 9001,9002,9003 };
	
	SOCKADDR_IN peeraddr;
	ZeroMemory(&peeraddr, sizeof(peeraddr));
	int peerlen = 0;
	UdpPacket udppacket;
	BOOL bEnable = TRUE;
	ZeroMemory(&udppacket, sizeof(udppacket));
	bool flag = false;
	PROTOCOL protocol;
	while (1)
	{
		if (flag&&udppacket.protocol!=PROTOCOL::MENU)
			break;
		switch (state)
		{
		case STATE::START:
			// 닉네임 입력하시오 전송.
			size = PackPacking(packet.Data, PROTOCOL::INPUTNICKNAME, NICKNAMEMSG);
			retval = send(user->sock, packet.Data, size, 0);
			if (retval == SOCKET_ERROR)
				err_display("send()");
			state = STATE::MENU;
			break;
		case STATE::MENU:
			//닉네임 받아오고 메뉴 전송.
			ZeroMemory(packet.Data, BUFSIZE);
			if (udppacket.protocol != PROTOCOL::MENU && udppacket.protocol!=PROTOCOL::END)
			{
				if (!RecvPacket(user->sock, packet.Data))
				{
					printf("데이터 받기 실패  state=menu\n");
					flag = true;
					break;
				}
				GetProtocol(packet.Data, &protocol);
				if (protocol == PROTOCOL::END)
				{
					state = STATE::END;
					break;
				}
				ZeroMemory(buf, BUFSIZE);
				UnPackPacket(packet.Data, buf);
				printf("[%s]님 접속\n", buf);
				ZeroMemory(packet.Data, BUFSIZE);
			}
			else if (udppacket.protocol == PROTOCOL::END)
			{
				state = STATE::END;
				break;
			}
			//메뉴 전송
			size = PackPacking(packet.Data, PROTOCOL::MENU, MENUSTRING);
			retval = send(user->sock, packet.Data, size, 0);
			if (retval == SOCKET_ERROR)
				err_display("send()");
			state = STATE::ADDR;
			break;
		case STATE::ADDR:
		{	
		ZeroMemory(packet.Data, BUFSIZE);
		if (!RecvPacket(user->sock, packet.Data))
		{
			printf("데이터 받기 실패 state=addr\n");
			flag = true;
			break;
		}
		GetProtocol(packet.Data, &protocol);
		if (protocol == PROTOCOL::END)
		{
			state = STATE::END;
			break;
		}
		ZeroMemory(buf, BUFSIZE);
		//메뉴 선택값에 따른 방 ip와 port 전송
		UnPackPacket(packet.Data, buf);
		menunumber = atoi(buf);
		ZeroMemory(packet.Data, BUFSIZE);
		switch (menunumber)
		{
		case 1:
			size = PackPacking(packet.Data, PROTOCOL::ADDR, IpArray[0], &PortArray[0]);
			retval = send(user->sock, packet.Data, size, 0);
			if (retval == SOCKET_ERROR)
				err_display("send()");
			break;
		case 2:
			size = PackPacking(packet.Data, PROTOCOL::ADDR, IpArray[1], &PortArray[1]);
			retval = send(user->sock, packet.Data, size, 0);
			if (retval == SOCKET_ERROR)
				err_display("send()");
			break;
		case 3:
			size = PackPacking(packet.Data, PROTOCOL::ADDR, IpArray[2], &PortArray[2]);
			retval = send(user->sock, packet.Data, size, 0);
			if (retval == SOCKET_ERROR)
				err_display("send()");
			break;
		default:
			printf("잘못된 입력값");
			//원래는 종료처리하지말고 에러메세지 클라에 전달. 밑의 return 0 은 임시코드
			return 0;
			break;
		}
		state = STATE::CHATTING;
		break;
		}
		case STATE::CHATTING:
			//udp 통신 소켓 생성
			udp.sock = socket(AF_INET, SOCK_DGRAM, 0);
			if (udp.sock == INVALID_SOCKET)
				err_display("socket()");
			udp.addr.sin_family = AF_INET;
			udp.addr.sin_addr.s_addr = inet_addr(SERVERIP);
			udp.addr.sin_port = htons(9005);

			//해줘야 같은 주소로 다른 클라가 접근시 또 바인딩 할 수 있음.
			retval = setsockopt(udp.sock, SOL_SOCKET, SO_REUSEADDR, (char*)&bEnable, sizeof(bEnable));
			if (retval == SOCKET_ERROR)
				err_quit("setsockopt()");

			retval = bind(udp.sock, (SOCKADDR*)&udp.addr, sizeof(udp.addr));
			if (retval == SOCKET_ERROR)
				err_display("bind()");

			//채팅시작
			//while (1)
			//{
				ZeroMemory(&udppacket, sizeof(udppacket));
				udppacket.protocol = PROTOCOL::NONE;
				peerlen = sizeof(peeraddr);
				//채팅종료 신호를 받아옵니다.
				retval = recvfrom(udp.sock, (char*)&udppacket, sizeof(udppacket), 0, (SOCKADDR*)&peeraddr, &peerlen);
				if (retval == SOCKET_ERROR)
				{
					err_display("recvfrom()");
					state = STATE::MENU;
					break;
				}
				//채팅종료 신호를 보낸 클라에 재 전송합니다.
				retval = sendto(udp.sock, (char*)&udppacket, sizeof(udppacket), 0, (SOCKADDR*)&peeraddr, peerlen);
				if (retval == SOCKET_ERROR)
				{
					err_display("sendto()");
					state = STATE::MENU;
					break;
				}
				//패킷에 전달된 프로토콜이 menu or end
				if (udppacket.protocol == PROTOCOL::MENU || udppacket.protocol == PROTOCOL::END)
				{
					flag = true;
				}
			//}
			//udp socket 반납
			closesocket(udp.sock);
			if (state == STATE::MENU)
				continue;
			//state를 메뉴로 변경
			if (flag && udppacket.protocol == PROTOCOL::MENU)
			{
				state = STATE::MENU;
				continue;
			}
			else if (flag)
			{
				state = STATE::END;
			}
			break;
		case STATE::END:
			flag = true;
			break;
		}
	}
	printf("[TCP/IP종료] ip:%s port:%d\n", inet_ntoa(user->addr.sin_addr), ntohs(user->addr.sin_port));
	closesocket(user->sock);
	return 0;
}

/* 문제점 한번 방에 들어갔다가 나왔을때 메뉴 출력까지는 잘 되는데 아직 클라에서 입력을 하지도 않았는데 state Add에서의 Recv()에서
   recv가 진행된다. 왜일까.일단 제출하고 다시 생각해보기.
   해야할것 방나가기,종료조건 좀더 좋게하기.*/

/* 교수님 그냥 머리가 안돌아가구 잘 못하네요 죄송해요 ㅠㅠ 계속 tcp랑 udp할 헷갈려서 
   채팅하다가 종료조건이 걸렸을때 채팅에서 서버udp주소로 send해서 protocol받고 받은 protocol을 다시 클라에 전송하는식으로 종료조건 주려고 했는데요
   udp서버와 멀티캐스트 주소에 대한 개념이 이해가 제대로 되지 않았어서 한참 삽질하다가 서버의 udp 주소로 보내고 싶으면 서버 udp socket에 대한
   ip,포트정보의 주소구조체에다가 보내야 한다는걸 깨달았네요.
   이 외에도 지금 꼬여서 종료조건도 완벽하지 않고 방나가기 후 재 입장에 대한 구현이 덜 되었는데 다 해서 다시 올리겠습니다.
   2022-03-31*/