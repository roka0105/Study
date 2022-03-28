#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

//지역브로드캐스트 주소
#define REMOTEIP   "255.255.255.255" 
#define REMOTEPORT 9000
#define BUFSIZE    512
#define ENDCOUNT 3
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
#define MESSAGE "<문제> 다음 중 최고의 플레이어는?\n1.홍창기\n2.박해민\n3.채은성\n4.김현수\n5.루이스\n"
struct ANSWER
{
	ANSWER()
	{
		number1 = 0;
		number2 = 0;
		number3 = 0;
		number4 = 0;
		number5 = 0;
	}
	int number1, number2, number3, number4, number5;
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

	// 브로드캐스팅 활성화
	BOOL bEnable =TRUE;
	retval = setsockopt(sock, SOL_SOCKET, SO_BROADCAST,
		(char *)&bEnable/*브로드캐스트 옵션 킴*/
		, sizeof(bEnable));
	if(retval == SOCKET_ERROR) err_quit("setsockopt()");

	// 소켓 주소 구조체 초기화
	SOCKADDR_IN remoteaddr;
	ZeroMemory(&remoteaddr, sizeof(remoteaddr));
	remoteaddr.sin_family = AF_INET;
	/*REMOTEIP= 255.255.255.255
	지금 여기 네트워크에 속한 모든 호스트의 REMOTEPORT 9000
	에 접근한다.*/
	remoteaddr.sin_addr.s_addr = inet_addr(REMOTEIP);
	remoteaddr.sin_port = htons(REMOTEPORT);
	
	/*
	bind를 안해주는데 socket 옵션으로 브로드캐스트 활성화 시킨 후
	REMOTEIP로 모든 아이피 접근하도록 해서 안하는것 같다.
	실행시 같은 네트워크에 속한 모든 호스트의 9000포트에 뿌려주기때문에
	받는 호스트 콘솔창이 열려 있어야 한다.
	*/

	// 데이터 통신에 사용할 변수
	char buf[BUFSIZE+1];
	int len=0;
	ANSWER answer;
	int number = 0;
	// 브로드캐스트 데이터 보내기
	ZeroMemory(buf, BUFSIZE);
	strcpy(buf, MESSAGE);
	while (1)
	{
		// 설문조사 질문지를 배포한다.
		retval = sendto(sock, buf, strlen(buf), 0,
			(SOCKADDR*)&remoteaddr, sizeof(remoteaddr));
		if (retval == SOCKET_ERROR) {
			err_display("sendto()1");
			continue;
		}
		printf("[UDP] %d바이트를 보냈습니다.\n", retval);
		int bufnumber = 0;
		len = sizeof(remoteaddr);
		//호스트에서 입력한 응답지를 받아온다.
		retval = recvfrom(sock, (char*)&bufnumber, sizeof(int), 0, (SOCKADDR*)&remoteaddr, &len);
		if (retval == SOCKET_ERROR)
		{
			err_display("recvfrom()");
			continue;
		}
		//받은 응답에 따라 몇표 받았는지 카운트 한다.
		switch (bufnumber)
		{
		case 1:
			answer.number1++;
			break;
		case 2:
			answer.number2++;
			break;
		case 3:
			answer.number3++;
			break;
		case 4:
			answer.number4++;
			break;
		case 5:
			answer.number5++;
			break;
		}
		//number는 변수이름을 잘못지었지만 몇명의 응답을 받았는지 카운트 한다.
		number++;
		//응답자가 ENDCOUNT 만큼이면 설문 종료.
		if (number == ENDCOUNT)
		{
			ZeroMemory(buf, BUFSIZE);
			sprintf(buf, "#설문조사 결과#\n1.홍창기:%d표\n2.박해민:%d표\n3.채은성:%d표\n4.김현수:%d표\n5.루이스:%d표\n"
				, answer.number1, answer.number2, answer.number3, answer.number4, answer.number5);
			printf("%s", buf);
			break;
		}
	}

	// closesocket()
	closesocket(sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}