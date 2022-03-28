#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

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

int main(int argc, char *argv[])
{
	int retval;

	// ���� �ʱ�ȭ
	WSADATA wsa;//���� ������ ���� ����ü WSADATA
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)//WSAStartup(���� ���� ����,output�� ����������) ��ȯ�� ������ 0
		return 1;

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);//���Ḹ ����� ������ �����. AF_INET IPv4,SOCK_STREAM TCP��
	if(listen_sock == INVALID_SOCKET) err_quit("socket()");//���� ���������� ��������� 0�̻� ��ȯ ���н� -1��ȯ.

	// bind()
	SOCKADDR_IN serveraddr;//IPv4 �����ּ� ����ü.
	ZeroMemory(&serveraddr, sizeof(serveraddr));//���� �ּ� ����ü �ʱ�ȭ 0���� �� �ʱ�ȭ��.
	serveraddr.sin_family = AF_INET;//�ּ�ü�� IPv4 �� ������.
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);//inet_addr("193.x.x...") ����. 
												   //INADDR_ANY �� ȣ��Ʈ�� �������� �ƹ��ų� ����. htonl=>����Ʈ����. l�� long
	serveraddr.sin_port = htons(SERVERPORT);//htons h ȣ��Ʈ n ��Ʈ��ũ s short  ����Ʈ�����ؼ� ��Ʈ�� �־����.
	retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));//���� ���ε� 
	if(retval == SOCKET_ERROR) err_quit("bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);//listen�� Ư���� ������ ����� �Լ��̴�. �������� ���Ḹ�� ����ϴ� �����̴�.
	//ť�� �����û�� ������� �״´�.
	if(retval == SOCKET_ERROR) err_quit("listen()");

	// ������ ��ſ� ����� ����
	SOCKET client_sock;//Ŭ���̾�Ʈ ����
	SOCKADDR_IN clientaddr;//IPv4 ���� �ּ� ����ü
	int addrlen;
	char buf[BUFSIZE+1];

	while(1){
		// accept()
		addrlen = sizeof(clientaddr);//Ŭ���̾�Ʈ ���� �ּ� ����ü ������ ����.
		client_sock = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);
		//Accept�� ������ ���� ��ȯ�Ѵ�. �׸��� ���� �Ű����� &clientaddr�� ����ü ���� ����.
		//���������� ť�� ���� ��û ������� ���� ���� ��ó�� �� ��� ��û��
	    //������ָ鼭 �� �����û�� �� ��(Ŭ���̾�Ʈ)�� ������ ���� �������ش�.

		if(client_sock == INVALID_SOCKET){
			err_display("accept()");
			break;
		}

		// ������ Ŭ���̾�Ʈ ���� ���
		printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
		//network to �ƽ�Ű
		// Ŭ���̾�Ʈ�� ������ ���
		while(1){
			// ������ �ޱ�
			retval = recv(client_sock, buf, BUFSIZE, 0);
			//ù��° �Ű������� ���Ź��ۿ��� �����͸� �޾ƿ�. ����� ���ú��� ����� �����̴�.
			//��� ����,���Ź��ۿ��� ������ ���� ���� ����,���Ź��ۿ��� �����ü� �ִ� �ִ�뷮,�÷���
			//���Ź��ۿ� ������ ������ ������ ���ö����� ���. �̷������� ����ϴ� ���� �Լ��� ���ȴ� ��� ǥ���Ѵ�.
            //�����͸� �����ְ�  \0�� �Ⱥ���
			if(retval == SOCKET_ERROR)//Ŭ���̾�Ʈ������ �ܼ��� â�ݱ������� ���Ź��ۿ� �������� ��Ŷ�� ���� 
				                      //���ú갡 �������� ��Ŷ�� �����´�.
			{
				err_display("recv()");
				break;
			}
			else if(retval == 0)//Ŭ���̾�Ʈ������ closesocket ������.(���ʿ��� �ƹ��͵� �Է¾��ϰ� ���������� ��������)
				
				break;

			// ���� ������ ���
			buf[retval] = '\0';
			printf("[TCP/%s:%d] %s\n", inet_ntoa(clientaddr.sin_addr),
				ntohs(clientaddr.sin_port), buf);

			// ������ ������
			retval = send(client_sock, buf, retval, 0);
			//�۽Ź��ۿ� �����迭�� ����ִ°��� retval��ŭ ������.
			//retval ���� �۽Ź��ۿ� ������ �뷮�� ���ϵȴ�.
			//���尡 ���Ǵ� ���� �۽Ź��۰� ������ �� �̻� ���� ������.
			if(retval == SOCKET_ERROR)//������ ��� �۽Ź��۰� �ݳ��Ǿ��־ �۽Ź��ۿ� ������ �����ص� ������ ������ �߻�.
			{
				err_display("send()");
				break;
			}
		}

		// closesocket()
		closesocket(client_sock);
		printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
	}

	// closesocket()
	closesocket(listen_sock);

	// ���� ����
	WSACleanup();
	return 0;
}