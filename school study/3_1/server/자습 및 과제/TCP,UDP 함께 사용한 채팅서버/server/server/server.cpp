#pragma comment (lib,"ws2_32")
//#include <Windows.h>
#include <winsock2.h>
#include <stdio.h>
#define BUFSIZE 512
#define SERVERIP "127.1.1.1"
#define SERVERPORT 9000
#define MENUSTRING "�޴��� �����ϼ���\r\n1.�ڿ��ù�\r\n2.����ȯ��\r\n3.��������"
#define NICKNAMEMSG "����Ͻ� �г��� �Է����ּ���\n"

DWORD WINAPI ClientThread(LPVOID arg);

enum class PROTOCOL
{
	NONE = -1,
	INPUTNICKNAME,
	MENU,
	ADDR,
	CHATTING,
	END,
	MAX
};
enum class STATE
{
	NONE = -1,
	START,
	MENU,
	ADDR,
	CHATTING,
	END,
	MAX
};
typedef struct NetworkPacket
{
	char Data[BUFSIZE];
	PROTOCOL protocol;
};
typedef struct UdpPacket
{
	PROTOCOL protocol;
	char name[BUFSIZE];
	char chat[BUFSIZE];
};
typedef struct UserInfo
{
	UserInfo()
	{
		ZeroMemory(nickname, BUFSIZE);
		ZeroMemory(&addr, sizeof(addr));
	}
	char nickname[BUFSIZE];
	SOCKET sock;
	SOCKADDR_IN addr;
};

void err_quit(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		0, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, (LPCTSTR)msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}
void err_display(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		0, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, lpMsgBuf);
	LocalFree(lpMsgBuf);
}
int recvn(SOCKET sock, char* buf, int len, bool flag)
{
	char* ptr = buf;
	int left = len;
	int recved = 0;
	while (left > 0)
	{
		recved = recv(sock, ptr, left, flag);
		if (recved == SOCKET_ERROR)
			return SOCKET_ERROR;
		if (recved == 0)
			break;
		left -= recved;
		ptr += recved;

	}
	return (len - left);
}
bool RecvPacket(SOCKET s, char* recvbuf)
{
	int size = 0;
	int retval = recvn(s, (char*)&size, sizeof(int), 0);
	if (retval == SOCKET_ERROR)
		return false;
	else if (retval == 0)
		return 0;
	retval = recvn(s, recvbuf, size, 0);
	if (retval == SOCKET_ERROR)return false;
	else if (retval == 0) return false;

	return true;
}
int PackPacking(char* buf, PROTOCOL protocol, const char* msg)
{
	char* ptr = buf + sizeof(int);
	int strsize = 0; 
	int size = 0;
	memcpy(ptr, &protocol, sizeof(PROTOCOL));
	size += sizeof(PROTOCOL);
	ptr += sizeof(PROTOCOL);
	if (msg != NULL)
	{
		strsize= strlen(msg);
		memcpy(ptr, &strsize, sizeof(int));
		ptr += sizeof(int);
		size += sizeof(int);

		memcpy(ptr, msg, strsize);
		ptr = buf;
		size += strsize;
	}
	memcpy(ptr, &size, sizeof(int));
	size += sizeof(int);
	return size;
}
int PackPacking(char* buf, PROTOCOL protocol, const char* msg, int* num)
{
	char* ptr = buf + sizeof(int);
	int strsize = strlen(msg);
	int size = 0;
	memcpy(ptr, &protocol, sizeof(PROTOCOL));
	size += sizeof(PROTOCOL);
	ptr += sizeof(PROTOCOL);
	memcpy(ptr, num, sizeof(int));
	size += sizeof(int);
	ptr += sizeof(int);
	if (msg != NULL)
	{
		memcpy(ptr, &strsize, sizeof(int));
		ptr += sizeof(int);
		size += sizeof(int);

		memcpy(ptr, msg, strsize);
		size += strsize;
	}
	ptr = buf;
	memcpy(ptr, &size, sizeof(int));
	size += sizeof(int);

	return size;
}
void UnPackPacket(const char* buf, char* msg)
{
	const char* ptr = buf + sizeof(PROTOCOL);
	int strsize = 0;
	memcpy(&strsize, ptr, sizeof(int));
	ptr += sizeof(int);
	memcpy(msg, ptr, strsize);
}
void GetProtocol(const char* buf, PROTOCOL* protocol)
{
	const char* ptr = buf;
	memcpy(protocol, ptr, sizeof(PROTOCOL));
}
int main(int argv, char* argc[])
{
	int retval = 0;
	WSADATA ws;
	if (WSAStartup(MAKEWORD(2, 2), &ws) != 0)
		return -1;

	//���ϻ��� �� ���ε�
	/*������ ������ �Ѵٰ� ����� ���� �ʴ´�.
	������ �ϴ� ���� bind�� �ؼ� Ŀ�ο� socket�� �÷����� ������ �����ϴ�.*/
	SOCKET listensock = socket(AF_INET, SOCK_STREAM, 0);
	if (listensock == INVALID_SOCKET)
		err_quit("socket()");
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listensock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		err_quit("bind()");
	retval = listen(listensock, SOMAXCONN);
	if (retval == SOCKET_ERROR)
		err_quit("listen()");

	SOCKET sock;
	int addrlen = sizeof(serveraddr);
	while (1)
	{
		//accept
		sock = accept(listensock, (SOCKADDR*)&serveraddr, &addrlen);
		if (sock == INVALID_SOCKET)
			err_quit("accept");

		printf("[TCP/IP ����] IP: %s PORT: %d\n", inet_ntoa(serveraddr.sin_addr), ntohs(serveraddr.sin_port));
		UserInfo* user = new UserInfo();
		ZeroMemory(user, sizeof(UserInfo));
		user->sock = sock;
		memcpy(&user->addr, &serveraddr, sizeof(serveraddr));

		//������ ����
		HANDLE hThread = CreateThread(0, NULL, ClientThread, (LPVOID)user, NULL, 0);
		if (hThread != NULL)
			CloseHandle(hThread);

	}

	WSACleanup();
	return 0;
}
DWORD WINAPI ClientThread(LPVOID arg)
{
	UserInfo* user = (UserInfo*)arg;
	UserInfo udp;
	NetworkPacket packet;
	ZeroMemory(&packet, sizeof(packet));
	STATE state = STATE::START;
	int size = 0;
	int retval = 0;
	char buf[BUFSIZE];
	int menunumber = 0;

	char IpArray[3][BUFSIZE] = { "224.1.1.1","224.1.1.2","224.1.1.3" };
	int PortArray[3] = { 9001,9002,9003 };
	
	SOCKADDR_IN peeraddr;
	ZeroMemory(&peeraddr, sizeof(peeraddr));
	int peerlen = 0;
	UdpPacket udppacket;
	BOOL bEnable = TRUE;
	ZeroMemory(&udppacket, sizeof(udppacket));
	bool flag = false;
	PROTOCOL protocol;
	while (1)
	{
		if (flag&&udppacket.protocol!=PROTOCOL::MENU)
			break;
		switch (state)
		{
		case STATE::START:
			// �г��� �Է��Ͻÿ� ����.
			size = PackPacking(packet.Data, PROTOCOL::INPUTNICKNAME, NICKNAMEMSG);
			retval = send(user->sock, packet.Data, size, 0);
			if (retval == SOCKET_ERROR)
				err_display("send()");
			state = STATE::MENU;
			break;
		case STATE::MENU:
			//�г��� �޾ƿ��� �޴� ����.
			ZeroMemory(packet.Data, BUFSIZE);
			if (udppacket.protocol != PROTOCOL::MENU && udppacket.protocol!=PROTOCOL::END)
			{
				if (!RecvPacket(user->sock, packet.Data))
				{
					printf("������ �ޱ� ����  state=menu\n");
					flag = true;
					break;
				}
				GetProtocol(packet.Data, &protocol);
				if (protocol == PROTOCOL::END)
				{
					state = STATE::END;
					break;
				}
				ZeroMemory(buf, BUFSIZE);
				UnPackPacket(packet.Data, buf);
				printf("[%s]�� ����\n", buf);
				ZeroMemory(packet.Data, BUFSIZE);
			}
			else if (udppacket.protocol == PROTOCOL::END)
			{
				state = STATE::END;
				break;
			}
			//�޴� ����
			size = PackPacking(packet.Data, PROTOCOL::MENU, MENUSTRING);
			retval = send(user->sock, packet.Data, size, 0);
			if (retval == SOCKET_ERROR)
				err_display("send()");
			state = STATE::ADDR;
			break;
		case STATE::ADDR:
		{	
		ZeroMemory(packet.Data, BUFSIZE);
		if (!RecvPacket(user->sock, packet.Data))
		{
			printf("������ �ޱ� ���� state=addr\n");
			flag = true;
			break;
		}
		GetProtocol(packet.Data, &protocol);
		if (protocol == PROTOCOL::END)
		{
			state = STATE::END;
			break;
		}
		ZeroMemory(buf, BUFSIZE);
		//�޴� ���ð��� ���� �� ip�� port ����
		UnPackPacket(packet.Data, buf);
		menunumber = atoi(buf);
		ZeroMemory(packet.Data, BUFSIZE);
		switch (menunumber)
		{
		case 1:
			size = PackPacking(packet.Data, PROTOCOL::ADDR, IpArray[0], &PortArray[0]);
			retval = send(user->sock, packet.Data, size, 0);
			if (retval == SOCKET_ERROR)
				err_display("send()");
			break;
		case 2:
			size = PackPacking(packet.Data, PROTOCOL::ADDR, IpArray[1], &PortArray[1]);
			retval = send(user->sock, packet.Data, size, 0);
			if (retval == SOCKET_ERROR)
				err_display("send()");
			break;
		case 3:
			size = PackPacking(packet.Data, PROTOCOL::ADDR, IpArray[2], &PortArray[2]);
			retval = send(user->sock, packet.Data, size, 0);
			if (retval == SOCKET_ERROR)
				err_display("send()");
			break;
		default:
			printf("�߸��� �Է°�");
			//������ ����ó���������� �����޼��� Ŭ�� ����. ���� return 0 �� �ӽ��ڵ�
			return 0;
			break;
		}
		state = STATE::CHATTING;
		break;
		}
		case STATE::CHATTING:
			//udp ��� ���� ����
			udp.sock = socket(AF_INET, SOCK_DGRAM, 0);
			if (udp.sock == INVALID_SOCKET)
				err_display("socket()");
			udp.addr.sin_family = AF_INET;
			udp.addr.sin_addr.s_addr = inet_addr(SERVERIP);
			udp.addr.sin_port = htons(9005);

			//����� ���� �ּҷ� �ٸ� Ŭ�� ���ٽ� �� ���ε� �� �� ����.
			retval = setsockopt(udp.sock, SOL_SOCKET, SO_REUSEADDR, (char*)&bEnable, sizeof(bEnable));
			if (retval == SOCKET_ERROR)
				err_quit("setsockopt()");

			retval = bind(udp.sock, (SOCKADDR*)&udp.addr, sizeof(udp.addr));
			if (retval == SOCKET_ERROR)
				err_display("bind()");

			//ä�ý���
			//while (1)
			//{
				ZeroMemory(&udppacket, sizeof(udppacket));
				udppacket.protocol = PROTOCOL::NONE;
				peerlen = sizeof(peeraddr);
				//ä������ ��ȣ�� �޾ƿɴϴ�.
				retval = recvfrom(udp.sock, (char*)&udppacket, sizeof(udppacket), 0, (SOCKADDR*)&peeraddr, &peerlen);
				if (retval == SOCKET_ERROR)
				{
					err_display("recvfrom()");
					state = STATE::MENU;
					break;
				}
				//ä������ ��ȣ�� ���� Ŭ�� �� �����մϴ�.
				retval = sendto(udp.sock, (char*)&udppacket, sizeof(udppacket), 0, (SOCKADDR*)&peeraddr, peerlen);
				if (retval == SOCKET_ERROR)
				{
					err_display("sendto()");
					state = STATE::MENU;
					break;
				}
				//��Ŷ�� ���޵� ���������� menu or end
				if (udppacket.protocol == PROTOCOL::MENU || udppacket.protocol == PROTOCOL::END)
				{
					flag = true;
				}
			//}
			//udp socket �ݳ�
			closesocket(udp.sock);
			if (state == STATE::MENU)
				continue;
			//state�� �޴��� ����
			if (flag && udppacket.protocol == PROTOCOL::MENU)
			{
				state = STATE::MENU;
				continue;
			}
			else if (flag)
			{
				state = STATE::END;
			}
			break;
		case STATE::END:
			flag = true;
			break;
		}
	}
	printf("[TCP/IP����] ip:%s port:%d\n", inet_ntoa(user->addr.sin_addr), ntohs(user->addr.sin_port));
	closesocket(user->sock);
	return 0;
}

/* ������ �ѹ� �濡 ���ٰ� �������� �޴� ��±����� �� �Ǵµ� ���� Ŭ�󿡼� �Է��� ������ �ʾҴµ� state Add������ Recv()����
   recv�� ����ȴ�. ���ϱ�.�ϴ� �����ϰ� �ٽ� �����غ���.
   �ؾ��Ұ� �泪����,�������� ���� �����ϱ�.*/

/* ������ �׳� �Ӹ��� �ȵ��ư��� �� ���ϳ׿� �˼��ؿ� �Ф� ��� tcp�� udp�� �򰥷��� 
   ä���ϴٰ� ���������� �ɷ����� ä�ÿ��� ����udp�ּҷ� send�ؼ� protocol�ް� ���� protocol�� �ٽ� Ŭ�� �����ϴ½����� �������� �ַ��� �ߴµ���
   udp������ ��Ƽĳ��Ʈ �ּҿ� ���� ������ ���ذ� ����� ���� �ʾҾ ���� �����ϴٰ� ������ udp �ּҷ� ������ ������ ���� udp socket�� ����
   ip,��Ʈ������ �ּұ���ü���ٰ� ������ �Ѵٴ°� ���޾ҳ׿�.
   �� �ܿ��� ���� ������ �������ǵ� �Ϻ����� �ʰ� �泪���� �� �� ���忡 ���� ������ �� �Ǿ��µ� �� �ؼ� �ٽ� �ø��ڽ��ϴ�.
   2022-03-31*/