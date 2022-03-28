#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

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

int main(int argc, char *argv[])
{
	int retval;

	// 윈속 초기화
	WSADATA wsa;//윈속 정보를 담을 구조체 WSADATA
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)//WSAStartup(윈속 버전 정보,output할 변수포인터) 반환형 성공시 0
		return 1;

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);//연결만 담당할 소켓을 만든다. AF_INET IPv4,SOCK_STREAM TCP형
	if(listen_sock == INVALID_SOCKET) err_quit("socket()");//소켓 성공적으로 만들어지면 0이상 반환 실패시 -1반환.

	// bind()
	SOCKADDR_IN serveraddr;//IPv4 소켓주소 구조체.
	ZeroMemory(&serveraddr, sizeof(serveraddr));//소켓 주소 구조체 초기화 0으로 다 초기화함.
	serveraddr.sin_family = AF_INET;//주소체계 IPv4 로 설정함.
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);//inet_addr("193.x.x...") 가능. 
												   //INADDR_ANY 이 호스트의 아이피중 아무거나 가능. htonl=>바이트정렬. l은 long
	serveraddr.sin_port = htons(SERVERPORT);//htons h 호스트 n 네트워크 s short  바이트정렬해서 포트를 넣어놓음.
	retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));//소켓 바인딩 
	if(retval == SOCKET_ERROR) err_quit("bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);//listen은 특별한 소켓을 만드는 함수이다. 서버에서 연결만을 담당하는 소켓이다.
	//큐에 연결요청을 순서대로 쌓는다.
	if(retval == SOCKET_ERROR) err_quit("listen()");

	// 데이터 통신에 사용할 변수
	SOCKET client_sock;//클라이언트 소켓
	SOCKADDR_IN clientaddr;//IPv4 소켓 주소 구조체
	int addrlen;
	char buf[BUFSIZE+1];

	while(1){
		// accept()
		addrlen = sizeof(clientaddr);//클라이언트 소켓 주소 구조체 사이즈 구함.
		client_sock = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);
		//Accept는 소켓을 만들어서 반환한다. 그리고 안의 매개변수 &clientaddr에 구조체 정보 저장.
		//리슨소켓의 큐에 연결 요청 대기중인 것을 보고 맨처음 온 대기 요청을
	    //허락해주면서 그 연결요청이 온 곳(클라이언트)의 소켓을 만들어서 리턴해준다.

		if(client_sock == INVALID_SOCKET){
			err_display("accept()");
			break;
		}

		// 접속한 클라이언트 정보 출력
		printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
		//network to 아스키
		// 클라이언트와 데이터 통신
		while(1){
			// 데이터 받기
			retval = recv(client_sock, buf, BUFSIZE, 0);
			//첫번째 매개변수의 수신버퍼에서 데이터를 받아옴. 샌드와 리시브의 대상은 버퍼이다.
			//대상 소켓,수신버퍼에서 가져온 내용 담을 공간,수신버퍼에서 가져올수 있는 최대용량,플래그
			//수신버퍼에 내용이 없으면 내용이 들어올때까지 대기. 이런식으로 대기하는 것을 함수가 블럭된다 라고 표현한다.
            //데이터만 보내주고  \0은 안보냄
			if(retval == SOCKET_ERROR)//클라이언트측에서 콘솔을 창닫기했을때 수신버퍼에 강제종료 패킷이 들어가고 
				                      //리시브가 강제종료 패킷을 가져온다.
			{
				err_display("recv()");
				break;
			}
			else if(retval == 0)//클라이언트측에서 closesocket 했을때.(저쪽에서 아무것도 입력안하고 엔터했을때 정상종료)
				
				break;

			// 받은 데이터 출력
			buf[retval] = '\0';
			printf("[TCP/%s:%d] %s\n", inet_ntoa(clientaddr.sin_addr),
				ntohs(clientaddr.sin_port), buf);

			// 데이터 보내기
			retval = send(client_sock, buf, retval, 0);
			//송신버퍼에 버프배열에 들어있는값을 retval만큼 보낸다.
			//retval 에는 송신버퍼에 쓰여진 용량이 리턴된다.
			//샌드가 블럭되는 경우는 송신버퍼가 꽉차서 더 이상 쓸수 없을때.
			if(retval == SOCKET_ERROR)//소켓이 없어서 송신버퍼가 반납되어있어서 송신버퍼에 무엇을 쓰려해도 쓸수가 없을때 발생.
			{
				err_display("send()");
				break;
			}
		}

		// closesocket()
		closesocket(client_sock);
		printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
	}

	// closesocket()
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}