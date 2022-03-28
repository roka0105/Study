#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERIP   "127.0.0.1"//������ �ּ� 
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

// ����� ���� ������ ���� �Լ�
int recvn(SOCKET s, char *buf, int len, int flags)//������ ��谡 ������ ���� �ʴ� Ư������ ����ϴ� �Լ�
                                                  //��ǥ�뷮������ �� �޾ƿ��ڴ�.��� �Լ��̴�. recv�� �ִ�ġ�뷮���� �۾Ƶ� �������� ����.
	                                              // ���� ���� Ŭ���̾�Ʈ������ ���������� �����Ͱ� �󸶳� �������� �˼����� 
												  // �׳� ���ú�� �޾� �´�� �޴´�.
	                                              //Ŭ�󿡼��� �뷮�� �� ���ؼ� ���ú��� �����°��̴�.
	                                              //�߿������� ��Ȯ�� �����Ʈ�� �޾ƾ������� �˾ƾ� �Ҷ��� ����ϴ°� recvn�̴�.
	                                              //�����Ʈ�� �޾ƾ����� �𸦶� ����ϸ� ���ɼ� �ִ�.
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

	// ���� �ʱ�ȭ dll�� �޸𸮿� �ε�.
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == INVALID_SOCKET) err_quit("socket()");

	// connect() 
	SOCKADDR_IN serveraddr;//Ŭ���̾�Ʈ�� ��û������ ������ ���������� ����.
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);//inet_addr �������� ip�� ���� (long)���� ip�� ��������.
	serveraddr.sin_port = SERVERPORT;
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));//�ڵ����ε� +�����û����
	//�������Ͽ� �����û�� ť�� ���� connect�� ������.������ ���� ����Ǵ� ������ �ƴ�. ���� ��û ��Ŷ�� ������ �ϴ°���.
	//�������� accept�� �Ǿ�� ������ �Ǵ°���.
	//�ڵ����ε��� �Ǹ� �� ���ึ�� ��Ʈ��ȣ�� �ٲ��. Ŭ��� ������ ��Ʈ��ȣ�� ���� �ʿ����. 
	//sock���� �ᱹ ȣ��Ʈ ip�ּҰ� ���� ��Ʈ�� ��밡���� ��Ʈ�� �ڵ����� �Ҵ�����.
	if(retval == SOCKET_ERROR) err_quit("connect()");
	//������ �ȿ��������ε� Ŭ�� �����ϸ� �ش� ������ ��ϴ�.

	// ������ ��ſ� ����� ����
	char buf[BUFSIZE+1];
	int len;

	// ������ ������ ���
	while(1)
	{
		// ������ �Է�
		printf("\n[���� ������] ");
		if(fgets(buf, BUFSIZE+1, stdin) == NULL)//gets�� ���Ͱ����� ���ϰ� scanf�� �� �����Է��� �ȵǾ 
			                                    //�Է¹����� ��� �����ϱ����� fgets���.
			                                    //stdin Ű����� �Է��ϰڴ�. ���������� ����Ǹ� null�� �ƴѰ� ����.
			break;
		 
		// '\n' ���� ����
		len = strlen(buf);//���ͱ��� �ԷµǾ��� ������ abcd\n\0�̸� 5�� len�� ��.
		if(buf[len-1] == '\n')
			buf[len-1] = '\0';//���͸� ����� \0�ֱ�.

		if(strlen(buf) == 0)//���͸� �Է��������� ��������� 0�� �Ǳ� ������ ���α׷��� ������.
			break;

		// ������ ������
		retval = send(sock, buf, strlen(buf), 0);
		if(retval == SOCKET_ERROR){
			err_display("send()");
			break;
		}
		printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� ���½��ϴ�.\n", retval);

		// ������ �ޱ�
		retval = recvn(sock, buf, retval, 0);
		if(retval == SOCKET_ERROR){
			err_display("recv()");
			break;
		}
		else if(retval == 0)
			break;

		// ���� ������ ���
		buf[retval] = '\0';
		printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� �޾ҽ��ϴ�.\n", retval);
		printf("[���� ������] %s\n", buf);
	}

	// closesocket()
	closesocket(sock);

	// ���� ����
	WSACleanup();
	return 0;
}