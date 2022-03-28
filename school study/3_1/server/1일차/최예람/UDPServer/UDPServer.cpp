#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERPORT 9000
#define BUFSIZE    512

// ���� �Լ� ���� ��� �� ����
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

// ���� �Լ� ���� ���
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
//���� ����ü
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
#define SUCCESSLOGIN "�α��μ���!"
#define FAILLOGIN "�α��ν���!"
int main(int argc, char* argv[])
{
	int retval;

	// ���� �ʱ�ȭ
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

	// ������ ��ſ� ����� ����
	SOCKADDR_IN clientaddr;
	int addrlen;
	char buf[BUFSIZE + 1];
	//ȸ������
	userinfo[0] = new UserInfo("1", "1");
	userinfo[1] = new UserInfo("2", "2");
	userinfo[2] = new UserInfo("3", "3");
	// Ŭ���̾�Ʈ�� ������ ���
	while (1)
	{
		// ������ �ޱ�
		UserInfo* currentuser = new UserInfo();
		addrlen = sizeof(clientaddr);
		//����ü�� Ŭ�󿡼� �Է��� ȸ�������� �޾ƿɴϴ�.
		retval = recvfrom(sock, (char*)currentuser, sizeof(UserInfo), 0,
			(SOCKADDR*)&clientaddr, &addrlen);
		if (retval == SOCKET_ERROR)
		{
			err_display("recvfrom()1");
			continue;
		}

		// ���� ������ ���
		printf("[UDP/%s:%d] id:%s pw:%s\n", inet_ntoa(clientaddr.sin_addr),
			ntohs(clientaddr.sin_port), currentuser->ID, currentuser->PW);
		RESULT result = RESULT::NONE;
		char msg[BUFSIZE];
		ZeroMemory(msg, BUFSIZE);
		bool flag = false;
		//ȸ����������ġ���� �˻��մϴ�.
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

		// �α��� ������� �����մϴ�.
		retval = sendto(sock, buf, sizeof(int)+strlen(msg), 0,
			(SOCKADDR*)&clientaddr, sizeof(clientaddr));
		if (retval == SOCKET_ERROR) {
			err_display("sendto()");
			continue;
		}
	}

	// closesocket()
	closesocket(sock);

	// ���� ����
	WSACleanup();
	return 0;
}