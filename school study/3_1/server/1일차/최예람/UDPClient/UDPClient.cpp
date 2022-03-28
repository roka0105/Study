#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERIP   "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE    512

// ���� �Լ� ���� ��� �� ����
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

// ���� �Լ� ���� ���
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

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock == INVALID_SOCKET) err_quit("socket()");

	// ���� �ּ� ����ü �ʱ�ȭ
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);

	// ������ ��ſ� ����� ����
	SOCKADDR_IN peeraddr;
	int addrlen;
	char ID[BUFSIZE],PW[BUFSIZE];
	int len;
	UserInfo* user;
	// ������ ������ ���
	while (1) {
		//ID �Է�
		printf("\n[ID] ");
		if (fgets(ID, BUFSIZE, stdin) == NULL)
			break;

		// '\n' ���� ����
		len = strlen(ID);
		if (ID[len - 1] == '\n')
			ID[len - 1] = '\0';
		if (strlen(ID) == 0)
			break;
		//ID �Է�
		printf("\n[PW] ");
		if (fgets(PW, BUFSIZE, stdin) == NULL)
			break;

		// '\n' ���� ����
		len = strlen(PW);
		if (PW[len - 1] == '\n')
			PW[len - 1] = '\0';
		if (strlen(PW) == 0)
			break;
		//�Է��� user������ ����ü�� �����մϴ�.
		user = new UserInfo(ID, PW);
		// �Է��� user���� ����ü�� �����մϴ�.
		//�̶� sizeof()�� user�� �ִ� �Ǽ����ߴµ� 
		//�����ʹ� 4byte�� �ѱ�� ������ �����ϸ� �߸��� ������
		//�����ϰ� �˴ϴ�. ���� ����ü�� ũ��� 1024�̹Ƿ� sizeof(*����ü)�� �ؾ�
		//UserInfo* user �� ����ü ũ�⸦ �����ü� �ֽ��ϴ�.
		
		/* ����ü �е������� ����ü °�� �����°� ��� �ٶ������� �ʴ�.
		   ���� �����Ϳ��� ���� ���� �е��� �޶����� ����ü ũ�Ⱑ �޶����� ����.
		   �е������� ���ִ� ������ �����ָ� ����.
		   ������ ����ü�� �Ұ����� �ɰ��� ������ �ٵ� �ɰ��� �����Ÿ�
		   ó���ϴ� ������ �� �ʿ��ϴ�.*/
		retval = sendto(sock, (char*)user, sizeof(*user), 0,
			(SOCKADDR*)&serveraddr, sizeof(serveraddr));
		if (retval == SOCKET_ERROR)
		{
			err_display("sendto()");
			continue;
		}
		printf("[UDP Ŭ���̾�Ʈ] %d����Ʈ�� ���½��ϴ�.\n", retval);

		char buf[BUFSIZE];
		char msg[BUFSIZE];
		ZeroMemory(buf, BUFSIZE);
		ZeroMemory(msg, BUFSIZE);
		//�α��� ����� �ޱ�
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

		// �۽����� IP �ּ� üũ
		if (memcmp(&peeraddr, &serveraddr, sizeof(peeraddr))) {
			printf("[����] �߸��� �������Դϴ�!\n");
			continue;
		}//�ٸ� �������� �����ؼ� �����͸� �������� �ִµ� 
		 //�װ��� ���� �ް��� �ϴ°����� ���� ���������� Ȯ���ϴ� �۾�.

		// ���� ������ ���
		buf[retval] = '\0';
		printf("[UDP Ŭ���̾�Ʈ] %d����Ʈ�� �޾ҽ��ϴ�.\n", retval);
		printf("[���� ������] %s\n", msg);
		if (result == RESULT::SUCCESS)
		{
			printf("���α׷� ����");
			break;
		}
	}

	// closesocket()
	closesocket(sock);

	// ���� ����
	WSACleanup();
	return 0;
}