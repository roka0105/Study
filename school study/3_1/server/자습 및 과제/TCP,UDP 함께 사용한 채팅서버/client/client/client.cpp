#pragma comment (lib,"ws2_32")
#include <winsock2.h>
#include <stdio.h>
#include "resource.h"
#include <ws2tcpip.h>
#define BUFSIZE 512
#define SERVERIP "127.1.1.1"
#define SERVERPORT 9000
#define MENUSTRING "�޴��� �����ϼ���\n1.�ڿ��ù�\n2.����ȯ��\n3.��������"
#define NICKNAMEMSG "����Ͻ� �г��� �Է����ּ���\n"

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
	char nickname[BUFSIZE];
	SOCKET sock;
	SOCKADDR_IN addr;
};

HWND _hDlg, NextroomBtn, OutRoomBtn, SendBtn, Edit, ShowEdit;
HANDLE hStartEvent, hSendEvent;
UserInfo user;
bool flag = false;
PROTOCOL protocol;
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
void GetProtocol(const char* buf, PROTOCOL* protocol)
{
	const char* ptr = buf;
	memcpy(protocol, buf, sizeof(PROTOCOL));
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
		size += strsize;
	}
    ptr = buf;
	memcpy(ptr, &size, sizeof(int));
	size += sizeof(int);
	return size;
}
int PackPacking(char* buf, PROTOCOL protocol, char* msg1, char* msg2)
{
	char* ptr = buf + sizeof(int);
	int strsize = strlen(msg1);
	int size = 0;
	memcpy(ptr, &protocol, sizeof(PROTOCOL));
	size += sizeof(PROTOCOL);
	ptr += sizeof(PROTOCOL);
	if (msg1 != NULL && msg2 != NULL)
	{
		memcpy(ptr, &strsize, sizeof(int));
		ptr += sizeof(int);
		size += sizeof(int);

		memcpy(ptr, msg1, strsize);
		ptr += strsize;
		size += strsize;

		strsize = strlen(msg2);
		memcpy(ptr, &strsize, sizeof(int));
		ptr += sizeof(int);
		size += sizeof(int);
		memcpy(ptr, msg2, strsize);
		size += strsize;
		ptr = buf;
	}

	memcpy(ptr, &size, sizeof(int));
	size += sizeof(int);
	return size;
}
void UnPackPacket(const char* buf, char* data)
{
	const char* ptr = buf + sizeof(PROTOCOL);
	int strsize = 0;
	memcpy(&strsize, ptr, sizeof(int));
	ptr += sizeof(int);
	memcpy(data, ptr, strsize);
}
void UnPackPacket(const char* buf, char* data1, char* data2)
{
	const char* ptr = buf + sizeof(PROTOCOL);
	int strsize = 0;
	memcpy(&strsize, ptr, sizeof(int));
	ptr += sizeof(int);
	memcpy(data1, ptr, strsize);
	ptr += strsize;
	memcpy(&strsize, ptr, sizeof(int));
	ptr += sizeof(int);
	memcpy(data2, ptr, strsize);
}
void UnPackPacket(const char* buf, int* data2, char* data1)
{
	const char* ptr = buf + sizeof(PROTOCOL);
	int strsize = 0;
	memcpy(data2, ptr, sizeof(int));
	ptr += sizeof(int);
	memcpy(&strsize, ptr, sizeof(int));
	ptr += sizeof(int);
	memcpy(data1, ptr, strsize);
}
DWORD WINAPI ClientThread(LPVOID);
DWORD WINAPI RecvThread(LPVOID);

INT_PTR CALLBACK DlgProc1(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	HANDLE hThread = CreateThread(0, NULL, ClientThread, NULL, 0, NULL);
	hStartEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	hSendEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc1);


	WSACleanup();
	return 0;
}
DWORD WINAPI ClientThread(LPVOID arg)
{
	int retval = 0;
	WSADATA ws;
	if (WSAStartup(MAKEWORD(2, 2), &ws) != 0)
		return -1;
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	SOCKADDR_IN addr;
	ZeroMemory(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(SERVERIP);
	addr.sin_port = htons(SERVERPORT);

	int addrlen = 0;
	NetworkPacket packet;

	ZeroMemory(&user, sizeof(user));
	ZeroMemory(&packet, sizeof(NetworkPacket));
	//connect
	retval = connect(sock, (SOCKADDR*)&addr, sizeof(addr));
	if (retval == SOCKET_ERROR)
		err_quit("connect()");

	char buf[BUFSIZE];
	int size = 0;
	int port = 0;
	int strsize = 0;

	SOCKET udpSock;
	SOCKADDR_IN udpaddr;

	BOOL bEnable = TRUE;
	ip_mreq mreq;
	SOCKADDR_IN remoteaddr;

	HANDLE hRecvThread;
	UdpPacket udppacket;
	ZeroMemory(&udppacket, sizeof(UdpPacket));
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(9005);

	bool end = false;
	while (1)
	{
		ZeroMemory(packet.Data, BUFSIZE);
		if (!RecvPacket(sock, packet.Data))
		{
			MessageBox(NULL, "�����͹ޱ� ����\n", "���Ž���", MB_OK);
			break;
		}
		GetProtocol(packet.Data, &packet.protocol);
		switch (packet.protocol)
		{
		case PROTOCOL::INPUTNICKNAME:
			WaitForSingleObject(hStartEvent, INFINITE);
			//�Է��Ͻÿ� ���
			ZeroMemory(buf, BUFSIZE);
			UnPackPacket(packet.Data, buf);

			SetWindowText(ShowEdit, buf);
			//�Է°��� �޴���û ����
			ZeroMemory(user.nickname, BUFSIZE);
			WaitForSingleObject(hSendEvent, INFINITE);
			if (protocol == PROTOCOL::END)
			{
				end = true;
				PackPacking(packet.Data, PROTOCOL::END, NULL);
				size = send(sock, packet.Data, size, 0);
				if (retval == SOCKET_ERROR)
					err_display("send()");
				break;
			}
			GetDlgItemText(_hDlg, IDC_EDIT1, user.nickname, BUFSIZE);
			SendMessage(Edit, EM_SETSEL, 0, BUFSIZE);
			SendMessage(Edit, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)"");
			strsize = strlen(user.nickname);
			size = PackPacking(packet.Data, PROTOCOL::MENU, user.nickname);
			retval = send(sock, packet.Data, size, 0);
			if (retval == SOCKET_ERROR)
				err_display("send()");
			break;
		case PROTOCOL::MENU:
			//�޾ƿ� �޴� ���
			ZeroMemory(buf, BUFSIZE);
			UnPackPacket(packet.Data, buf);
			SetWindowText(ShowEdit, buf);
			WaitForSingleObject(hSendEvent, INFINITE);
			if (protocol == PROTOCOL::END)
			{
				end = true;
				PackPacking(packet.Data, PROTOCOL::END,NULL);
				size = send(sock, packet.Data, size, 0);
				if (retval == SOCKET_ERROR)
					err_display("send()");
				break;
			}
			ZeroMemory(buf, BUFSIZE);
			ZeroMemory(packet.Data, BUFSIZE);
			//������Ʈ�ѿ��� �Է°��� ������ editâ ����.
			GetDlgItemText(_hDlg, IDC_EDIT1, buf, BUFSIZE);
			SendMessage(Edit, EM_SETSEL, 0, BUFSIZE);
			SendMessage(Edit, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)"");
			//�� ���� �� �� ���� ����
			PackPacking(packet.Data, PROTOCOL::ADDR, buf);
			size = send(sock, packet.Data, size, 0);
			if (retval == SOCKET_ERROR)
				err_display("send()");
			break;
		case PROTOCOL::ADDR:
			EnableWindow(OutRoomBtn, TRUE);
			//�ּҹ޾ƿ�
			ZeroMemory(buf, BUFSIZE);
			UnPackPacket(packet.Data, &port, buf);
			strsize = GetWindowTextLength(ShowEdit);
			//�޴� ����ߴ��� ���ڿ� �����
			SendMessage(ShowEdit, EM_SETSEL, 0, strsize);
			SendMessage(ShowEdit, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)"");
			//tcp ������ �״�� �ΰ� udp ������ �����ؾ��ҰŰ�����
			udpSock = socket(AF_INET, SOCK_DGRAM, 0);
			if (udpSock == INVALID_SOCKET)
				err_quit("socket()");

			// ���� ������ ���� ������ �����Ҷ� Ŀ�ο��� ����ó��.������ ���
			retval = setsockopt(udpSock, SOL_SOCKET, SO_REUSEADDR, (char*)&bEnable, sizeof(bEnable));
			if (retval == SOCKET_ERROR)
				err_quit("setsockopt()");

			//��Ƽĳ��Ʈ ��� ���
			mreq.imr_multiaddr.s_addr = inet_addr(buf);//��Ƽĳ��Ʈ �ּ�
			mreq.imr_interface.s_addr = htonl(INADDR_ANY);//�� �ּ�
			retval = setsockopt(udpSock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq));
			if (retval == SOCKET_ERROR)
				err_quit("setsockopt()");

			//recv�ޱ� ���� ���ε�.
			ZeroMemory(&udpaddr, sizeof(udpaddr));
			udpaddr.sin_family = AF_INET;
			udpaddr.sin_addr.s_addr = htonl(INADDR_ANY);
			udpaddr.sin_port = htons(port);
			retval = bind(udpSock, (SOCKADDR*)&udpaddr, sizeof(udpaddr));
			if (retval == SOCKET_ERROR)
				err_quit("bind()");

			//send �ּұ���ü
			ZeroMemory(&remoteaddr, sizeof(remoteaddr));
			remoteaddr.sin_family = AF_INET;
			remoteaddr.sin_addr.s_addr = inet_addr(buf);
			remoteaddr.sin_port = htons(port);

			//�ޱ����� ������
			hRecvThread = CreateThread(0, NULL, RecvThread, (void*)udpSock, 0, NULL);

			//ä��
			while (1)
			{
				//���̾�α׿��� ��ȣ�ۿ�Ű�� ������ Ȱ��ȭ (������,�泪����)
				WaitForSingleObject(hSendEvent, INFINITE);
				//�泪���� �̸� while�� Ż��
				if (flag)
					break;
				ZeroMemory(buf, BUFSIZE);
				ZeroMemory(packet.Data, BUFSIZE);
				//�������� ���� ����
				GetDlgItemText(_hDlg, IDC_EDIT1, buf, BUFSIZE);
				//���ڼ� ī��Ʈ
				strsize = GetWindowTextLength(Edit);
				//���ڼ� ��ŭ �����
				SendMessage(Edit, EM_SETSEL, 0, strsize);
				SendMessage(Edit, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)"");
				//�г��Ӱ� ��ȭ���� ����
				strcpy(udppacket.name, user.nickname);
				strcpy(udppacket.chat, buf);
				//remoteaddr�� ��Ƽĳ��Ʈ �ּ�.
				retval = sendto(udpSock, (char*)&udppacket, sizeof(udppacket), 0, (SOCKADDR*)&remoteaddr, sizeof(remoteaddr));
				if (retval == SOCKET_ERROR)
					err_display("sendto()");
			}
			//�泪���� �� �� �����ϱ� �Ǵ� �����⸦ ������ Ȱ��ȭ.
			WaitForSingleObject(hStartEvent, INFINITE);

			//��Ŷ�� protocol�� ������ ���� udp�� ����
			if (protocol == PROTOCOL::MENU)
			{
				udppacket.protocol = PROTOCOL::MENU;
			}
			else if (protocol == PROTOCOL::END)
			{
				udppacket.protocol = PROTOCOL::END;
			}
			retval = sendto(udpSock, (char*)&udppacket, sizeof(udppacket), 0, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
			if (retval == SOCKET_ERROR)
				err_display("send()");

			//������ ������ protocol�� �ٽ� recvthread�� ���� thread�� ������Ѿ� wait�� Ǯ��.
			WaitForSingleObject(hRecvThread, INFINITE);
	
			//���Ż�� �� ��������
			retval = setsockopt(udpSock, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*)&mreq, sizeof(mreq));
			if (retval == SOCKET_ERROR)
				err_display("setsockopt()");

	
			closesocket(udpSock);
			break;
		case PROTOCOL::END:
			end = true;
			break;
		}
		if (end)
			break;
	}
	MessageBox(NULL, "���ν����� ����", "����Ȯ��", MB_OK);

	closesocket(sock);
	return 0;
}
INT_PTR CALLBACK DlgProc1(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int strsize = 0;
	switch (uMsg)
	{
	case WM_INITDIALOG:
		_hDlg = hDlg;
		SendBtn = GetDlgItem(hDlg, IDOK);
		NextroomBtn = GetDlgItem(hDlg, IDNEXTROOM);
		OutRoomBtn = GetDlgItem(hDlg, IDOUTROOM);
		Edit = GetDlgItem(hDlg, IDC_EDIT1);
		ShowEdit = GetDlgItem(hDlg, IDC_EDIT2);
		EnableWindow(NextroomBtn, FALSE);
		EnableWindow(SendBtn, TRUE);
		EnableWindow(OutRoomBtn, FALSE);
		SetEvent(hStartEvent);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			SetEvent(hSendEvent);
			return TRUE;
		case IDOUTROOM:
			EnableWindow(NextroomBtn, TRUE);
			EnableWindow(SendBtn, FALSE);
			flag = true;
			SetEvent(hSendEvent);
			return TRUE;
		case IDNEXTROOM:
			protocol = PROTOCOL::MENU;
			EnableWindow(NextroomBtn, FALSE);
			EnableWindow(SendBtn, TRUE);
			EnableWindow(OutRoomBtn, FALSE);
			SetEvent(hStartEvent);
			return TRUE;
		case IDEND:
			flag = true;
			protocol = PROTOCOL::END;
			SetEvent(hSendEvent);
			SetEvent(hStartEvent);
			EndDialog(hDlg, IDEND);
			return TRUE;
		}
		return FALSE;
	case WM_CLOSE:
		flag = true;
		protocol = PROTOCOL::END;
		SetEvent(hSendEvent);
		SetEvent(hStartEvent);
		EndDialog(hDlg, WM_CLOSE);
		return TRUE;
	}
	return FALSE;
}
DWORD WINAPI RecvThread(LPVOID arg)
{
	SOCKET sock = (SOCKET)arg;
	SOCKADDR_IN multipeeraddr;
	int peeraddrlen = sizeof(multipeeraddr);
	UdpPacket udppacket;

	int retval = 0;
	ZeroMemory(&multipeeraddr, sizeof(multipeeraddr));
	int txtlen = 0;
	char buf[BUFSIZE];
	char temp[BUFSIZE];
	char name[BUFSIZE];
	//timeval timeo = { 3,500000 };

	//setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeo, sizeof(timeo));
	while (1)
	{
		ZeroMemory(name, BUFSIZE);
		ZeroMemory(temp, BUFSIZE);
		ZeroMemory(buf, BUFSIZE);
		ZeroMemory(&udppacket, sizeof(udppacket));
		ZeroMemory(udppacket.name, BUFSIZE);
		ZeroMemory(udppacket.chat, BUFSIZE);
		
		//������ ������ protocol ��Ŷ�� ����� ���� ����ǰ� �ƴϸ� ��Ƽĳ��Ʈ���� �Ѹ��� �����͸� ����.
		retval = recvfrom(sock, (char*)&udppacket, sizeof(udppacket), 0, (SOCKADDR*)&multipeeraddr, &peeraddrlen);
		if (retval == SOCKET_ERROR)
		{
			err_display("recvfrom()");
			MessageBox(NULL, "����������", "Ȯ��", MB_OK);
		}
		else
		{
			//�������� �� ���������� ������ ���
			//menu�� �泪���� �� �漱���ϱ⸦ ������ ��� �ݺ��� Ż�� �� ������ ����
			if (udppacket.protocol == PROTOCOL::MENU)
			{
				SendMessage(ShowEdit, EM_SETSEL, txtlen, txtlen);
				SendMessage(ShowEdit, EM_REPLACESEL, (WPARAM)FALSE, (LPARAM)"������ ����");
				break;
			}//������ �ݺ��� Ż�� �� ������ ����
			else if (udppacket.protocol == PROTOCOL::END)
			{
				MessageBox(NULL, "����������", "Ȯ��", MB_OK);
				return 0;
			}
			//������ �� �ɸ���� �׳� �Ϲ� ä���� ����̹Ƿ� �г��Ӱ� ��ȭ ���
			sprintf(buf, "[%s] %s \r\n", udppacket.name, udppacket.chat);
			txtlen = GetWindowTextLength(ShowEdit);

			SendMessage(ShowEdit, EM_SETSEL, txtlen, txtlen);
			SendMessage(ShowEdit, EM_REPLACESEL, (WPARAM)FALSE, (LPARAM)buf);
		}
	}

	return 0;
}