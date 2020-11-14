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


//전역변수 
int SysEnd = 1;   // 채팅기능 온오프 표시 1은 on 0은 off

void f_recv(SOCKET* m_hSock, char* m_cBuffer, char* m_cMsg)
{
	while (1)
	{

		recv(*m_hSock, m_cBuffer, PACKET_SIZE, 0);
		

		if (m_cBuffer[0] == '/' && m_cBuffer[1] == 'Q')   // /Q 입력시  서버 클라이언트 종료 .
		{
			send(*m_hSock, m_cBuffer, strlen(m_cMsg), 0);
			SysEnd = 0;
			break;
		}
		else if (m_cBuffer[0] != 0)
		{
			printf(" 서버 : %s \n", m_cBuffer);
			memset(m_cBuffer, 0, (size_t)PACKET_SIZE);
		}
		

		

	}
	return  ;
}


void f_send(SOCKET* m_hSock, char* m_cBuffer, char* m_cMsg)
{
	while (1)
	{

		//채팅기능의 핵심 

		printf(" 입력 : ");

		//scanf("%s \n", cMsg);     
		//scanf는 문자입력후에 한번더 문자입력해야함 

		cin >> m_cMsg;

		if (SysEnd == 0) { system("cls"); printf(" 시스템 종료 "); break; } // 시스템이 꺼지면 보내는 기능도 정지

		send(*m_hSock, m_cMsg, strlen(m_cMsg), 0);



	}
	return ;
}

char IPv4[12] = { 0, };
int main()
{
	cout << "서버 주소를 입력하세요 : ";
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

	// 버퍼와  메세지 선언 

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
