#include <stdio.h>
#include <WinSock2.h>
#include <iostream>
#include <thread>

#pragma comment(lib,"ws2_32")


#define PORT 4579
#define PACKET_SIZE 1024
#define SERVER_IP "192.168.219.104"
//192.168.219.104

using namespace std;


//�������� 
int SysEnd = 1;   // ä�ñ�� �¿��� ǥ�� 1�� on 0�� off

void f_recv(SOCKET* m_hSock, char* m_cBuffer, char* m_cMsg)
{
	while (1)
	{

		recv(*m_hSock, m_cBuffer, PACKET_SIZE, 0);
		

		if (m_cBuffer[0] == '/' && m_cBuffer[1] == 'Q')   // /Q �Է½�  ���� Ŭ���̾�Ʈ ���� .
		{
			send(*m_hSock, m_cBuffer, strlen(m_cMsg), 0);
			SysEnd = 0;
			break;
		}
		else if (m_cBuffer[0] != 0)
		{
			printf(" ���� : %s \n", m_cBuffer);
			memset(m_cBuffer, 0, (size_t)PACKET_SIZE);
		}
		

		

	}
	return  ;
}


void f_send(SOCKET* m_hSock, char* m_cBuffer, char* m_cMsg)
{
	while (1)
	{

		//ä�ñ���� �ٽ� 

		printf(" �Է� : ");

		//scanf("%s \n", cMsg);     
		//scanf�� �����Է��Ŀ� �ѹ��� �����Է��ؾ��� 

		cin >> m_cMsg;

		if (SysEnd == 0) { system("cls"); printf(" �ý��� ���� "); break; } // �ý����� ������ ������ ��ɵ� ����

		send(*m_hSock, m_cMsg, strlen(m_cMsg), 0);



	}
	return ;
}

char IPv4[12] = { 0, };
int main()
{
	cout << "���� �ּҸ� �Է��ϼ��� : ";
	cin >> IPv4;
	system("cls");


	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET hSocket;
	hSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN tAddr = {};
	tAddr.sin_family = AF_INET;
	tAddr.sin_port = htons(PORT);
	tAddr.sin_addr.s_addr = inet_addr(IPv4);

	connect(hSocket, (SOCKADDR*)&tAddr, sizeof(tAddr));

	// ���ۿ�  �޼��� ���� 

	char cMsg[] = "Client Send";


	char cBuffer[PACKET_SIZE] = {};

	thread Recv(f_recv, &hSocket, cBuffer, cMsg);
	thread Send(f_send, &hSocket, cBuffer, cMsg);

	

	Recv.join();
	Send.join();

	closesocket(hSocket);

	WSACleanup();

	return 0;
}
