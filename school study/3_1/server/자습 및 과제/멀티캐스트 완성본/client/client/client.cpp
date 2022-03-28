#pragma comment (lib,"ws2_32")
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#define BUFSIZE 512
#define SERVERIP "127.0.0.1"
#define PORT 9000

ip_mreq mreq;
//종료 플래그
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
	{   //스레드종료
		if (flag)
		{
			printf("스레드 정상종료");
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

	//sand할때 사용하기 위한 이곳의 주소
	SOCKADDR_IN localaddr;
	ZeroMemory(&localaddr, sizeof(localaddr));
	localaddr.sin_family = AF_INET;
	localaddr.sin_addr.s_addr = inet_addr(SERVERIP);
	localaddr.sin_port = htons(PORT);

	//소켓이 종료해도 바로 소멸하는것이 아니라 또 소켓을 생성할때 같은 곳에 접근하면 커널에서 오류처리하는데
	//그것을 SO_REUSEADDR로 재사용 허용하여 에러가 안나게 설정함.
	BOOL bEnable = TRUE;
	retval = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
		(char*)&bEnable
		, sizeof(bEnable));
	//상대주소 담을 구조체
	SOCKADDR_IN peeraddr;
	ZeroMemory(&peeraddr, sizeof(peeraddr));
	int peerlen = sizeof(peeraddr);
	char buf[BUFSIZE];
	ZeroMemory(buf, BUFSIZE);


	Packet packet;
	packet.protocol = PROTOCOL::MENU;
	int room_number = 0;
	//packet에 담긴 정보와 localaddr(내주소정보들)을 전송+자동바인딩.
	retval = sendto(sock, (char*)&packet, sizeof(packet), 0, (SOCKADDR*)&localaddr, sizeof(localaddr));
	if (retval == SOCKET_ERROR)
		err_display((char*)"sendto()");
	//서버에서 메뉴정보를 받아옴,서버주소정보를 peeraddr에 받아옴.recvfrom()
	retval = recvfrom(sock, (char*)&packet, sizeof(packet), 0, (SOCKADDR*)&peeraddr, &peerlen);
	if (retval == SOCKET_ERROR)
		err_display((char*)"recvfrom");
	if (sizeof(peeraddr) != peerlen)
	{
		printf("잘못된 연결\n");
	}
	//메뉴에 대한 유저의 답을 받아옴.
	while (1)
	{
		printf("%s\n", packet.buf);
		ZeroMemory(packet.buf, BUFSIZE);
		scanf("%d", &packet.number);
		//1~3번이 아닌 수 입력시 다시 입력하도록 처리
		if (packet.number > 3 || packet.number <= 0)
			printf("다시입력하세요!\n");
		else
		{
			//유저이름받기
			printf("유저이름입력:");
			scanf("%s", packet.username);
			getchar();
			break;
		}
	}
	packet.protocol = PROTOCOL::ADDRESS;
	//주소 요청
	retval = sendto(sock, (char*)&packet, sizeof(packet), 0, (SOCKADDR*)&localaddr, sizeof(localaddr));
	if (retval == SOCKET_ERROR)
		err_display((char*)"sendto()");
	//주소 받아옴
	retval = recvfrom(sock, (char*)&packet, sizeof(packet), 0, (SOCKADDR*)&peeraddr, &peerlen);
	if (retval == SOCKET_ERROR)
		err_display((char*)"recvfrom1()");
	//기존에 쓰던 sock 없애기.
	closesocket(sock);
	sock = INVALID_SOCKET;
	//멀티캐스트에서 사용할 소켓 생성.
	SOCKET recv_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (recv_sock == INVALID_SOCKET) err_display((char*)"socket()");

	//가입할주소= 서버에서 받아온 멀티캐스트 주소
	mreq.imr_multiaddr.s_addr = inet_addr(packet.buf);
	//내주소= 아무주소
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	//멀티캐스트 멤버등록
	retval = setsockopt(recv_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq));
	if (retval == SOCKET_ERROR)err_quit((char*)"setsockopt()");

	//recv_sock에 대한 REUSE 설정.
	bEnable = TRUE;
	retval = setsockopt(recv_sock, SOL_SOCKET, SO_REUSEADDR, (char*)&bEnable, sizeof(bEnable));

	//recv받기 위해 바인딩 작업.
	SOCKADDR_IN multiaddr;
	ZeroMemory(&multiaddr, sizeof(multiaddr));
	multiaddr.sin_family = AF_INET;
	//아무 아이피나 넣음.
	multiaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	//서버에서 받아온 방에대한 포트번호
	multiaddr.sin_port = htons(packet.number);
	retval = bind(recv_sock, (SOCKADDR*)&multiaddr, sizeof(multiaddr));
	if (retval == SOCKET_ERROR)
		err_display((char*)"bind()");

	//send를 위한 주소구조체
	SOCKADDR_IN remoteaddr;
	ZeroMemory(&remoteaddr, sizeof(remoteaddr));
	remoteaddr.sin_family = AF_INET;
	remoteaddr.sin_addr.s_addr = inet_addr(packet.buf);
	remoteaddr.sin_port = htons(packet.number);

	//멀티스레드 만들기
	HANDLE hThread = CreateThread(NULL, 0, RecvThread, (LPVOID)recv_sock, 0, NULL);

	packet.protocol = PROTOCOL::INCHATTING;
	
	//채팅
	while (1)
	{
		ZeroMemory(packet.buf, BUFSIZE);
		
		//scanf("%s", packet.buf);
		if (fgets(packet.buf, BUFSIZE, stdin) == nullptr) 
		{
			return 0;
		}
		//end입력시 프로그램 종료
		if (!strcmp(packet.buf, "end\n"))
		{
			flag = true;
			printf("프로그램 종료\n");
			break;
		}
		//서버에 입력한 채팅 내용 전송
		retval = sendto(recv_sock, (char*)&packet, sizeof(packet), 0, (SOCKADDR*)&remoteaddr, sizeof(remoteaddr));
		if (retval == SOCKET_ERROR)
			err_display((char*)"sendtooo()");
	}
	//멤버 해제
	retval = setsockopt(recv_sock, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*)&mreq, sizeof(mreq));
	if (retval == SOCKET_ERROR)err_quit((char*)"setsockopt()");
	//소켓반납
	closesocket(recv_sock);


	//윈속 사용종료
	WSACleanup();
	return 0;
}
/* 너무 어려워요 ㅠㅠ 70퍼까지는 혼자했는데 그 뒤부터 모르겠고 어렵고 막혀가지고 계속찾아보고
주변사람들한테 물어보고 해서 풀긴 풀었는데 정확히 왜 그런건지 이해하고 푼게 아닌것같아서 너무 슬프네요 ㅠㅠ*/