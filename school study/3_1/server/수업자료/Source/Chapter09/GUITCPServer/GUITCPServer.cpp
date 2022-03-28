#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERPORT 9000
#define BUFSIZE    512

// 윈도우 프로시저
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
// 편집 컨트롤 출력 함수
void DisplayText(char *fmt, ...);
// 오류 출력 함수
void err_quit(char *msg);
void err_display(char *msg);
// 소켓 통신 스레드 함수
DWORD WINAPI ServerMain(LPVOID arg);
DWORD WINAPI ProcessClient(LPVOID arg);

HINSTANCE hInst; // 인스턴스 핸들
HWND hEdit; // 편집 컨트롤
CRITICAL_SECTION cs; // 임계 영역

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
	hInst = hInstance;
	//크리티컬섹션(임계영역) 초기화
	InitializeCriticalSection(&cs);

	// 윈도우 클래스 등록
	WNDCLASS wndclass;
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = "MyWndClass";
	if(!RegisterClass(&wndclass)) return 1;

	// 윈도우 생성
	HWND hWnd = CreateWindow("MyWndClass", "TCP 서버", WS_OVERLAPPEDWINDOW,
		0, 0, 600, 200, NULL, NULL, hInstance, NULL);
	if(hWnd == NULL) return 1;
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// 소켓 통신 스레드 생성
	CreateThread(NULL, 0, ServerMain, NULL, 0, NULL);

	// 메시지 루프
	MSG msg;
	while(GetMessage(&msg, 0, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	//크리티컬섹션(임계영역) 종료
	DeleteCriticalSection(&cs);
	return msg.wParam;
}

// 윈도우 프로시저
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg){
	case WM_CREATE:
		//window나 컨트롤러를 만드는 함수이다
		//edit형태로 ws_child ... 스타일로 만들며 0,0,0,0은 위치값, hWnd는  생성할 이 윈도우의 부모핸들
		//HMENU는 식별 ID,hInst 윈도우 생성할 인스턴스 핸들
		hEdit = CreateWindow("edit", NULL, 
			WS_CHILD | WS_VISIBLE | WS_HSCROLL | 
			WS_VSCROLL | ES_AUTOHSCROLL | 
			ES_AUTOVSCROLL | ES_MULTILINE | ES_READONLY,
			0, 0, 0, 0, hWnd, (HMENU)100, hInst, NULL);
		return 0;
		//WM_SIZE에서 윈도우 크기가 변경될 때마다 작업 영역의 크기를 다시 계산하고 화면을 
		//다시 그리므로 문자열은 윈도우의 크기에 상관없이 항상 같은 위치에 있게 된다.
		//윈도우 크기에 상관없이 일정한 레이아웃을 유지하기 위해 사용.
	case WM_SIZE:
		//윈도우를 특정위치 특정 크기로 변경시키는 함수
		//2~3번째 이동할 좌표,4~5 적용할 크기지정,6번째 매개변수는 적용하면 이 window를 새로 그릴지.
		MoveWindow(hEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
		return 0;
	case WM_SETFOCUS:
		//포커스를 가져갈 윈도우 핸들.
		//에딧창에 포커스 (깜빡이는 작대기)가 있음.
		SetFocus(hEdit);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// 편집 컨트롤 출력 함수
void DisplayText(char *fmt, ...)
{
	//가변 인수들에 대한 정보를 홀드하기 위한 타입
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
	//임계영역 진입합니다.
	//다른 스레드들은 이곳의 cs 열쇠가 반납되기 전까지는 실행할 수 없습니다.
	//한번의 한 Thread.
	EnterCriticalSection(&cs);
	//제목 문자열 크기를 구하기 위한 함수이다. 매개변수는 window handle을 전달한다.
	//실패시 0반환.
	int nLength = GetWindowTextLength(hEdit);
	//SendMessage 는 강제로 메세지를 발생시킴.
	//인자로는 어떤 창에, 어떤 메시지를,어떤 파라미터값으로 발생시킬지.
	//문자열 선택 뒤의 매개변수 두개는 문자열 시작,끝
	SendMessage(hEdit, EM_SETSEL, nLength, nLength);
	//문자열 교체
	SendMessage(hEdit, EM_REPLACESEL, FALSE, (LPARAM)cbuf);
	//임계영역에서 빠져나옵니다. 열쇠 반납.
	LeaveCriticalSection(&cs);
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

// TCP 서버 시작 부분
DWORD WINAPI ServerMain(LPVOID arg)
{
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(listen_sock == INVALID_SOCKET) err_quit("socket()");

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if(retval == SOCKET_ERROR) err_quit("bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if(retval == SOCKET_ERROR) err_quit("listen()");

	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	HANDLE hThread;

	while(1){
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);
		if(client_sock == INVALID_SOCKET){
			err_display("accept()");
			break;
		}

		// 접속한 클라이언트 정보 출력
		DisplayText("\r\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\r\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		// 클라이언트 관리 스레드 생성
		hThread = CreateThread(NULL, 0, ProcessClient,
			(LPVOID)client_sock, 0, NULL);
		//핸들 할당 실패하면 소켓 반납.
		if(hThread == NULL) { closesocket(client_sock); }
		//핸들 할당 성공(쓰레드 성공적으로 만들어지면) 쓰레드 권한을 반납.
		else { CloseHandle(hThread); }
	}

	// closesocket()
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}

// 클라이언트와 데이터 통신
DWORD WINAPI ProcessClient(LPVOID arg)
{
	SOCKET client_sock = (SOCKET)arg;
	int retval;
	SOCKADDR_IN clientaddr;
	int addrlen;
	char buf[BUFSIZE+1];

	// 클라이언트 정보 얻기
	addrlen = sizeof(clientaddr);
	//함수는 소켓이 연결되어 있는 원격지 상대방(peer)의 이름(인터페이스 어드레스와 포트번호)을 얻는 함수
	getpeername(client_sock, (SOCKADDR *)&clientaddr, &addrlen);

	while(1){
		// 데이터 받기
		retval = recv(client_sock, buf, BUFSIZE, 0);
		if(retval == SOCKET_ERROR){
			err_display("recv()");
			break;
		}
		else if(retval == 0)
			break;

		// 받은 데이터 출력
		buf[retval] = '\0';
		//그냥 sprintf를 다른 형식으로 쓰는 방법이라고 생각하면 될듯하다.
		DisplayText("[TCP/%s:%d] %s\r\n", inet_ntoa(clientaddr.sin_addr),
			ntohs(clientaddr.sin_port), buf);

		// 받은 데이터 다시 보내기
		retval = send(client_sock, buf, retval, 0);
		if(retval == SOCKET_ERROR){
			err_display("send()");
			break;
		}
	}

	// closesocket()
	closesocket(client_sock);
	DisplayText("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\r\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	return 0;
}