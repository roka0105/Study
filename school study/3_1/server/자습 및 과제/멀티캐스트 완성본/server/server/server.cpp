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
	//멀티캐스트에서 사용할 주소와 포트번호를 가지고 있습니다.
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

	//아이피와 포트번호를 addrlist라는 배열에 넣어두고 사용합니다.
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
	//받아온 상대 주소 넣을 공간.
	SOCKADDR_IN peeraddr;
	ZeroMemory(&peeraddr, sizeof(peeraddr));
	int peerlen = sizeof(peeraddr);
	char buf[BUFSIZE];
	ZeroMemory(buf, BUFSIZE);
	Packet packet;
	while (1)
	{
		ZeroMemory(&packet, sizeof(packet));
		//클라에게서 정보를 받아오며 클라주소를 peeraddr에 저장.
		retval = recvfrom(sock, (char*)&packet, sizeof(packet), 0, (SOCKADDR*)&peeraddr, &peerlen);
		if (retval == SOCKET_ERROR)
			err_display((char*)"recvfrom");
		//protocol에 따라 수행하는 명령이 다름.
		switch (packet.protocol)
		{   //메뉴 요청일때
			case PROTOCOL::MENU:
			sprintf(packet.buf, "%s", "1.박용택\n2.오지환\n3.김현수\n입력:");
			//메뉴 정보를 클라에게 전송한다. sendto()
			retval = sendto(sock, (char*)&packet, sizeof(packet), 0, (SOCKADDR*)&peeraddr, sizeof(peeraddr));
			if (retval == SOCKET_ERROR)
				err_display((char*)"sendto()");
			printf("[UDP] %d 바이트 보냈습니다.\n", retval);
			break;
			//주소 요청일때
			case PROTOCOL::ADDRESS:
				//클라에서 입력한 방 번호에 따라서 서버에서 어느방에 입장했는지 정보를 출력하고
				// 멀티캐스트에서 사용할 ip와 port번호를 전송한다.
				switch (packet.number)
				{
				case 1:
					printf("박용택방 %s님 입장\n",packet.username);
					//ip저장
					strcpy(packet.buf, addr[0].buf);
					break;
				case 2:
					printf("오지환방 %s님 입장\n", packet.username);
					//ip저장
					strcpy(packet.buf, addr[1].buf);
					break;
				case 3:
					printf("김현수방 %s님 입장\n", packet.username);
					//ip저장
					strcpy(packet.buf, addr[2].buf);
					break;
				}
				//port번호저장
				packet.number = port[packet.number - 1];
				retval = sendto(sock, (char*)&packet, sizeof(packet), 0, (SOCKADDR*)&peeraddr, sizeof(peeraddr));
				if (retval == SOCKET_ERROR)
				{
					err_display((char*)"sendto()");
			    }
				break;
				//채팅중일때
			case PROTOCOL::INCHATTING:
				//packet에 담긴 buf(채팅),username(유저이름)을 전송한다.
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