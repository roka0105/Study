#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERIP   "127.0.0.1"//루프백 주소 
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

// 사용자 정의 데이터 수신 함수
int recvn(SOCKET s, char *buf, int len, int flags)//데이터 경계가 구분이 되지 않는 특성땜에 사용하는 함수
                                                  //목표용량까지는 꼭 받아오겠다.라는 함수이다. recv는 최대치용량보다 작아도 가져오고 끝남.
	                                              // 에코 서버 클라이언트에서는 서버에서는 데이터가 얼마나 들어오는지 알수없고 
												  // 그냥 리시브로 받아 온대로 받는다.
	                                              //클라에서는 용량을 딱 정해서 리시븐을 돌리는것이다.
	                                              //중요한점은 정확히 몇바이트를 받아야할지를 알아야 할때만 사용하는게 recvn이다.
	                                              //몇바이트를 받아야할지 모를때 사용하면 블럭될수 있다.
{
	int received;
	char *ptr = buf;
	int left = len;

	while(left > 0){
		received = recv(s, ptr, left, flags);
		if(received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if(received == 0)
			break;
		left -= received;
		ptr += received;
	}

	return (len - left);
}

int main(int argc, char *argv[])
{
	int retval;

	// 윈속 초기화 dll을 메모리에 로드.
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == INVALID_SOCKET) err_quit("socket()");

	// connect() 
	SOCKADDR_IN serveraddr;//클라이언트가 요청을보낼 서버의 리슨소켓의 정보.
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);//inet_addr 문자형의 ip를 숫자 (long)형의 ip로 변경해줌.
	serveraddr.sin_port = SERVERPORT;
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));//자동바인딩 +연결요청보냄
	//리슨소켓에 연결요청이 큐에 들어가면 connect가 리턴함.하지만 실제 연결되는 시점은 아님. 연결 요청 패킷을 접수만 하는것임.
	//서버에서 accept가 되어야 연결이 되는것임.
	//자동바인딩이 되면 매 실행마다 포트번호가 바뀐다. 클라는 고정된 포트번호가 굳이 필요없다. 
	//sock에는 결국 호스트 ip주소가 들어가고 포트는 사용가능한 포트를 자동으러 할당해줌.
	if(retval == SOCKET_ERROR) err_quit("connect()");
	//서버가 안열린상태인데 클라를 실행하면 해당 에러가 뜹니다.

	// 데이터 통신에 사용할 변수
	char buf[BUFSIZE+1];
	int len;

	// 서버와 데이터 통신
	while(1)
	{
		// 데이터 입력
		printf("\n[보낼 데이터] ");
		if(fgets(buf, BUFSIZE+1, stdin) == NULL)//gets는 엔터검출을 못하고 scanf는 띄어서 글자입력이 안되어서 
			                                    //입력받은걸 모두 검출하기위해 fgets사용.
			                                    //stdin 키보드로 입력하겠다. 정상적으로 실행되면 null이 아닌값 리턴.
			break;
		 
		// '\n' 문자 제거
		len = strlen(buf);//엔터까지 입력되었기 때문에 abcd\n\0이면 5가 len에 들어감.
		if(buf[len-1] == '\n')
			buf[len-1] = '\0';//엔터를 지우고 \0넣기.

		if(strlen(buf) == 0)//엔터만 입력했을때는 결과적으로 0이 되기 때문에 프로그램이 끝난다.
			break;

		// 데이터 보내기
		retval = send(sock, buf, strlen(buf), 0);
		if(retval == SOCKET_ERROR){
			err_display("send()");
			break;
		}
		printf("[TCP 클라이언트] %d바이트를 보냈습니다.\n", retval);

		// 데이터 받기
		retval = recvn(sock, buf, retval, 0);
		if(retval == SOCKET_ERROR){
			err_display("recv()");
			break;
		}
		else if(retval == 0)
			break;

		// 받은 데이터 출력
		buf[retval] = '\0';
		printf("[TCP 클라이언트] %d바이트를 받았습니다.\n", retval);
		printf("[받은 데이터] %s\n", buf);
	}

	// closesocket()
	closesocket(sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}