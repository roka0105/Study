#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

//������ε�ĳ��Ʈ �ּ�
#define REMOTEIP   "255.255.255.255" 
#define REMOTEPORT 9000
#define BUFSIZE    512
#define ENDCOUNT 3
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
#define MESSAGE "<����> ���� �� �ְ��� �÷��̾��?\n1.ȫâ��\n2.���ع�\n3.ä����\n4.������\n5.���̽�\n"
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

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock == INVALID_SOCKET) err_quit("socket()");

	// ��ε�ĳ���� Ȱ��ȭ
	BOOL bEnable =TRUE;
	retval = setsockopt(sock, SOL_SOCKET, SO_BROADCAST,
		(char *)&bEnable/*��ε�ĳ��Ʈ �ɼ� Ŵ*/
		, sizeof(bEnable));
	if(retval == SOCKET_ERROR) err_quit("setsockopt()");

	// ���� �ּ� ����ü �ʱ�ȭ
	SOCKADDR_IN remoteaddr;
	ZeroMemory(&remoteaddr, sizeof(remoteaddr));
	remoteaddr.sin_family = AF_INET;
	/*REMOTEIP= 255.255.255.255
	���� ���� ��Ʈ��ũ�� ���� ��� ȣ��Ʈ�� REMOTEPORT 9000
	�� �����Ѵ�.*/
	remoteaddr.sin_addr.s_addr = inet_addr(REMOTEIP);
	remoteaddr.sin_port = htons(REMOTEPORT);
	
	/*
	bind�� �����ִµ� socket �ɼ����� ��ε�ĳ��Ʈ Ȱ��ȭ ��Ų ��
	REMOTEIP�� ��� ������ �����ϵ��� �ؼ� ���ϴ°� ����.
	����� ���� ��Ʈ��ũ�� ���� ��� ȣ��Ʈ�� 9000��Ʈ�� �ѷ��ֱ⶧����
	�޴� ȣ��Ʈ �ܼ�â�� ���� �־�� �Ѵ�.
	*/

	// ������ ��ſ� ����� ����
	char buf[BUFSIZE+1];
	int len=0;
	ANSWER answer;
	int number = 0;
	// ��ε�ĳ��Ʈ ������ ������
	ZeroMemory(buf, BUFSIZE);
	strcpy(buf, MESSAGE);
	while (1)
	{
		// �������� �������� �����Ѵ�.
		retval = sendto(sock, buf, strlen(buf), 0,
			(SOCKADDR*)&remoteaddr, sizeof(remoteaddr));
		if (retval == SOCKET_ERROR) {
			err_display("sendto()1");
			continue;
		}
		printf("[UDP] %d����Ʈ�� ���½��ϴ�.\n", retval);
		int bufnumber = 0;
		len = sizeof(remoteaddr);
		//ȣ��Ʈ���� �Է��� �������� �޾ƿ´�.
		retval = recvfrom(sock, (char*)&bufnumber, sizeof(int), 0, (SOCKADDR*)&remoteaddr, &len);
		if (retval == SOCKET_ERROR)
		{
			err_display("recvfrom()");
			continue;
		}
		//���� ���信 ���� ��ǥ �޾Ҵ��� ī��Ʈ �Ѵ�.
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
		//number�� �����̸��� �߸��������� ����� ������ �޾Ҵ��� ī��Ʈ �Ѵ�.
		number++;
		//�����ڰ� ENDCOUNT ��ŭ�̸� ���� ����.
		if (number == ENDCOUNT)
		{
			ZeroMemory(buf, BUFSIZE);
			sprintf(buf, "#�������� ���#\n1.ȫâ��:%dǥ\n2.���ع�:%dǥ\n3.ä����:%dǥ\n4.������:%dǥ\n5.���̽�:%dǥ\n"
				, answer.number1, answer.number2, answer.number3, answer.number4, answer.number5);
			printf("%s", buf);
			break;
		}
	}

	// closesocket()
	closesocket(sock);

	// ���� ����
	WSACleanup();
	return 0;
}