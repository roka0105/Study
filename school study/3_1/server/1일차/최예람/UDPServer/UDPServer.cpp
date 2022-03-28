#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERPORT 9000
#define BUFSIZE    512

// 소켓 함수 오류 출력 후 종료
void err_quit(char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// 소켓 함수 오류 출력
void err_display(char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}
//유저 구조체
struct UserInfo
{
	UserInfo()
	{
		ZeroMemory(ID, BUFSIZE);
		ZeroMemory(PW, BUFSIZE);
	}
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
UserInfo* userinfo[3];
#define SUCCESSLOGIN "로그인성공!"
#define FAILLOGIN "로그인실패!"
int main(int argc, char* argv[])
{
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	// 데이터 통신에 사용할 변수
	SOCKADDR_IN clientaddr;
	int addrlen;
	char buf[BUFSIZE + 1];
	//회원정보
	userinfo[0] = new UserInfo("1", "1");
	userinfo[1] = new UserInfo("2", "2");
	userinfo[2] = new UserInfo("3", "3");
	// 클라이언트와 데이터 통신
	while (1)
	{
		// 데이터 받기
		UserInfo* currentuser = new UserInfo();
		addrlen = sizeof(clientaddr);
		//구조체에 클라에서 입력한 회원정보를 받아옵니다.
		retval = recvfrom(sock, (char*)currentuser, sizeof(UserInfo), 0,
			(SOCKADDR*)&clientaddr, &addrlen);
		if (retval == SOCKET_ERROR)
		{
			err_display("recvfrom()1");
			continue;
		}

		// 받은 데이터 출력
		printf("[UDP/%s:%d] id:%s pw:%s\n", inet_ntoa(clientaddr.sin_addr),
			ntohs(clientaddr.sin_port), currentuser->ID, currentuser->PW);
		RESULT result = RESULT::NONE;
		char msg[BUFSIZE];
		ZeroMemory(msg, BUFSIZE);
		bool flag = false;
		//회원정보와일치한지 검사합니다.
		for (int i = 0; i < 3; ++i)
		{
			if (!strcmp(currentuser->ID, userinfo[i]->ID))
			{
				if (!strcmp(currentuser->PW, userinfo[i]->PW))
				{
					result = RESULT::SUCCESS;
					strcpy(msg, SUCCESSLOGIN);
					flag = true;
					break;
				}
			}
			result = RESULT::FAIL;
			strcpy(msg, FAILLOGIN);
		}
		ZeroMemory(buf, BUFSIZE);
		memcpy(buf, &result, sizeof(RESULT));
		strcpy(buf + sizeof(RESULT), msg);

		// 로그인 결과값을 전송합니다.
		retval = sendto(sock, buf, sizeof(int)+strlen(msg), 0,
			(SOCKADDR*)&clientaddr, sizeof(clientaddr));
		if (retval == SOCKET_ERROR) {
			err_display("sendto()");
			continue;
		}
	}

	// closesocket()
	closesocket(sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}