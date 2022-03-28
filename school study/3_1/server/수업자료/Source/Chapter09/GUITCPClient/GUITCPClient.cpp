#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include "resource.h"

#define SERVERIP   "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE    512

// 대화상자 프로시저
BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
// 편집 컨트롤 출력 함수
void DisplayText(char *fmt, ...);
// 오류 출력 함수
void err_quit(char *msg);
void err_display(char *msg);
// 사용자 정의 데이터 수신 함수
int recvn(SOCKET s, char *buf, int len, int flags);
// 소켓 통신 스레드 함수
DWORD WINAPI ClientMain(LPVOID arg);

SOCKET sock; // 소켓
char buf[BUFSIZE+1]; // 데이터 송수신 버퍼
HANDLE hReadEvent, hWriteEvent; // 이벤트
HWND hSendButton; // 보내기 버튼
HWND hEdit1, hEdit2; // 편집 컨트롤

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
	// 이벤트 생성
	//읽기 이벤트를 수동으로 초기값을 true로 주며 시작한다.
	hReadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	if(hReadEvent == NULL) return 1;
	//쓰기 이벤트를 수동으로 초기값을 false로 주며 시작한다.
	hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if(hWriteEvent == NULL) return 1;

	// 소켓 통신 스레드 생성
	CreateThread(NULL, 0, ClientMain, NULL, 0, NULL);

	// 대화상자 생성 리소스에 만들어놓은 IDD_DIALOG와 실행할 기능이 들어있는 DlgProc랑 연결.
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);

	// 이벤트 제거
	CloseHandle(hReadEvent);
	CloseHandle(hWriteEvent);

	// closesocket()
	closesocket(sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}

// 대화상자 프로시저
BOOL CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg){
		//다이어그램 초기화
	case WM_INITDIALOG:
		//edit창 1의 제어핸들을 받아옵니다.
		hEdit1 = GetDlgItem(hDlg, IDC_EDIT1);
		//edit창 2의 제어핸들을 받아옵니다.
		hEdit2 = GetDlgItem(hDlg, IDC_EDIT2);
		//버튼 OK클릭에 대한 제어 핸들 받아옵니다.
		hSendButton = GetDlgItem(hDlg, IDOK);
		//bufsize만큼 글자수를 제한합니다.
		SendMessage(hEdit1, EM_SETLIMITTEXT, BUFSIZE, 0);
		return TRUE;
	case WM_COMMAND:
		switch(LOWORD(wParam)){
		case IDOK:
			EnableWindow(hSendButton, FALSE); // 보내기 버튼 비활성화
			WaitForSingleObject(hReadEvent, INFINITE); // 읽기 완료 기다리기
			//해당 edit에 써진 글자를 읽어와서 buf에 저장.
			GetDlgItemText(hDlg, IDC_EDIT1, buf, BUFSIZE+1);
			SetEvent(hWriteEvent); // 쓰기 완료 알리기
			//포커스를 다시 IDC_EDIT1에 둔다.
			SetFocus(hEdit1);
			//EM_SETSEL은 선택 영역의 시작과 끝점을 변경 하는 메세지/
			//wParam으로 시작점, lParam으로 끝점을 전달합니다.wParam에 0, lParam에 - 1을 넣으면 전체 선택
			//wParam이 - 1이면 선택영역을 해제하게 된답니다
			SendMessage(hEdit1, EM_SETSEL, 0, -1);
			return TRUE;
		case IDCANCEL:
			//다이어로그 닫기.
			EndDialog(hDlg, IDCANCEL);
			return TRUE;
		}
		//어떤 메세지도 처리 안했다면 false;
		return FALSE;
	}
	return FALSE;
}

// 편집 컨트롤 출력 함수
void DisplayText(char *fmt, ...)
{//가변 인수들에 대한 정보를 홀드하기 위한 타입
	va_list arg;
	//va_start=va_list 를 초기화 하기 위해 사용 
	//첫번째 가변인자의 주소를 알려면 고정인수인 두번째 매개변수가 필요하다.
	//va_list 주소값에 고정인수를 더한 위치로 arg의 시작 위치를 지정해줍니다.
	va_start(arg, fmt);

	char cbuf[BUFSIZE+256];
	//첫번째 매개변수:문자열 담을 버퍼, 두번째 매개변수:fmt 문자열,세번째 매개변수:나머지 인자들
	//위에서 설정해 놓은 fmt 다음 위치에 arg 메모리 시작주소를 해놨기 때문에 ...에 해당하는
	//매개변수들을 불러와서 매칭시킵니다.
	vsprintf(cbuf, fmt, arg);
	//제목 문자열 크기를 구하기 위한 함수이다. 매개변수는 window handle을 전달한다.
	//실패시 0반환.
	int nLength = GetWindowTextLength(hEdit2);
	//SendMessage 는 강제로 메세지를 발생시킴.
	//인자로는 어떤 창에, 어떤 메시지를,어떤 파라미터값으로 발생시킬지.
	//문자열 선택 뒤의 매개변수 두개는 문자열 시작,끝
	SendMessage(hEdit2, EM_SETSEL, nLength, nLength);
	//문자열 교체
	SendMessage(hEdit2, EM_REPLACESEL, FALSE, (LPARAM)cbuf);
	//vs_list 타입을 null로 바꿉니다.
	va_end(arg);
}

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
	DisplayText("[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

// 사용자 정의 데이터 수신 함수
int recvn(SOCKET s, char *buf, int len, int flags)
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

// TCP 클라이언트 시작 부분
DWORD WINAPI ClientMain(LPVOID arg)
{
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return 1;

	// socket()
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == INVALID_SOCKET) err_quit("socket()");

	// connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if(retval == SOCKET_ERROR) err_quit("connect()");

	// 서버와 데이터 통신
	while(1)
	{
		WaitForSingleObject(hWriteEvent, INFINITE); // 쓰기 완료 기다리기

		// 문자열 길이가 0이면 보내지 않음 
		//글자입력이 안된상태이므로 continue로 다시 while문위로 올라가서 사용자가 edit에
		//새로운 무언가를 입력할때까지 wait상태로 대기한다.
		if(strlen(buf) == 0)
		{
			EnableWindow(hSendButton, TRUE); // 보내기 버튼 활성화
			SetEvent(hReadEvent); // 읽기 완료 알리기
			continue;
		}

		// edit에 입력한 글자 데이터 보내기
		retval = send(sock, buf, strlen(buf), 0);
		if(retval == SOCKET_ERROR){
			err_display("send()");
			break;
		}
		//sprint 기능을 구현한 함수.
		DisplayText("[TCP 클라이언트] %d바이트를 보냈습니다.\r\n", retval);

		//보낸 데이터를 다시 받기
		retval = recvn(sock, buf, retval, 0);
		if(retval == SOCKET_ERROR){
			err_display("recv()");
			break;
		}
		else if(retval == 0)
			break;

		// 받은 데이터 출력
		buf[retval] = '\0';
		DisplayText("[TCP 클라이언트] %d바이트를 받았습니다.\r\n", retval);
		DisplayText("[받은 데이터] %s\r\n", buf);

		EnableWindow(hSendButton, TRUE); // 보내기 버튼 활성화
		SetEvent(hReadEvent); // 읽기 완료 알리기
		//입력이 정상적일때 흐름은 edit에서 true로 생성되었던 readevent가 실행된체로 내려가서 입력을 받으며
		//writeevent를 켜준다.(입력완료했다고 알림) 
		//그리고 쓰레드로 와서 다이어로그 edit에서 입력한 값을 서버에 전송 후 보낸 바이트 수를 displaytext에서
		//edit2에 sendmessage를 통해 메세지 출력합니다.
		//그 후 서버에서 다시 보냈던 데이터 정보를 받아오고 바이트 수등을 displaytext를 통해
		//edit2에 다시 재 출력 후 정보를 다 읽어왔다고 readevent를 켜주고 다이어로그의 readwait을 풀어줍니다.
	}

	return 0;
}