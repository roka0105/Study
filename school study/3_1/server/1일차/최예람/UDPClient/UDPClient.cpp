#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERIP   "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE    512

// 소켓 함수 오류 출력 후 종료
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

// 소켓 함수 오류 출력
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
struct UserInfo
{
	UserInfo(const char* id, const char* pw)
	{
		ZeroMemory(ID, BUFSIZE);
		ZeroMemory(PW, BUFSIZE);
		strcpy(ID, id);
		strcpy(PW, pw);
	}
	char ID[BUFSIZE];
	char PW[BUFSIZE];
};
enum class RESULT
{
	NONE = 0,
	SUCCESS = 1,
	FAIL
};
int main(int argc, char *argv[])
{
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock == INVALID_SOCKET) err_quit("socket()");

	// 소켓 주소 구조체 초기화
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);

	// 데이터 통신에 사용할 변수
	SOCKADDR_IN peeraddr;
	int addrlen;
	char ID[BUFSIZE],PW[BUFSIZE];
	int len;
	UserInfo* user;
	// 서버와 데이터 통신
	while (1) {
		//ID 입력
		printf("\n[ID] ");
		if (fgets(ID, BUFSIZE, stdin) == NULL)
			break;

		// '\n' 문자 제거
		len = strlen(ID);
		if (ID[len - 1] == '\n')
			ID[len - 1] = '\0';
		if (strlen(ID) == 0)
			break;
		//ID 입력
		printf("\n[PW] ");
		if (fgets(PW, BUFSIZE, stdin) == NULL)
			break;

		// '\n' 문자 제거
		len = strlen(PW);
		if (PW[len - 1] == '\n')
			PW[len - 1] = '\0';
		if (strlen(PW) == 0)
			break;
		//입력한 user정보를 구조체에 저장합니다.
		user = new UserInfo(ID, PW);
		// 입력한 user정보 구조체를 전송합니다.
		//이때 sizeof()에 user를 넣는 실수를했는데 
		//포인터는 4byte로 넘기기 때문에 전송하면 잘못된 정보를
		//전송하게 됩니다. 현재 구조체의 크기는 1024이므로 sizeof(*구조체)로 해야
		//UserInfo* user 의 구조체 크기를 가져올수 있습니다.
		
		/* 구조체 패딩때문에 구조체 째로 보내는건 사실 바람직하지 않다.
		   같은 데이터여도 선언에 따라 패딩이 달라져서 구조체 크기가 달라질수 있음.
		   패딩공간을 없애는 과정을 거쳐주면 좋음.
		   묶을때 구조체로 할것인지 쪼개서 보낼지 근데 쪼개서 보낼거면
		   처리하는 과정이 좀 필요하다.*/
		retval = sendto(sock, (char*)user, sizeof(*user), 0,
			(SOCKADDR*)&serveraddr, sizeof(serveraddr));
		if (retval == SOCKET_ERROR)
		{
			err_display("sendto()");
			continue;
		}
		printf("[UDP 클라이언트] %d바이트를 보냈습니다.\n", retval);

		char buf[BUFSIZE];
		char msg[BUFSIZE];
		ZeroMemory(buf, BUFSIZE);
		ZeroMemory(msg, BUFSIZE);
		//로그인 결과값 받기
		addrlen = sizeof(peeraddr);
		retval = recvfrom(sock, buf, BUFSIZE, 0,
			(SOCKADDR*)&peeraddr, &addrlen);
		RESULT result = RESULT::NONE;
		memcpy(&result, buf, sizeof(RESULT));
		strcpy(msg, buf + sizeof(RESULT));
		if (retval == SOCKET_ERROR) {
			err_display("recvfrom()");
			continue;
		}

		// 송신자의 IP 주소 체크
		if (memcmp(&peeraddr, &serveraddr, sizeof(peeraddr))) {
			printf("[오류] 잘못된 데이터입니다!\n");
			continue;
		}//다른 서버에서 접근해서 데이터를 보낼수도 있는데 
		 //그것을 막고 받고자 하는곳에서 보낸 데이터인지 확인하는 작업.

		// 받은 데이터 출력
		buf[retval] = '\0';
		printf("[UDP 클라이언트] %d바이트를 받았습니다.\n", retval);
		printf("[받은 데이터] %s\n", msg);
		if (result == RESULT::SUCCESS)
		{
			printf("프로그램 종료");
			break;
		}
	}

	// closesocket()
	closesocket(sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}