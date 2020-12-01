#include <stdio.h>
#include <WinSock2.h>
#include <iostream>
#include <thread>


#include <string>
#include <windows.h>



#pragma comment(lib,"ws2_32")



//192.168.219.104

using namespace std;


//�������� 
static int SysEnd = 1;   // ä�ñ�� �¿��� ǥ�� 1�� on 0�� off

char IPv4[16] = { 0, };  // ����  IPv4 �ּ� 

const int PacketSize = 1024;  // ��� ��Ŷ������ 
const int Port = 4579;        // ��� ��Ʈ ��ȣ 

const UINT ChatLine = 22;     // ä���� ���� �� 

static HANDLE hCursor = GetStdHandle(STD_OUTPUT_HANDLE); // �ܼ� Ŀ���� ���� �ڵ� 

COORD gPos = { 0,0 };

//Ŭ���� ���� ���� 

class Manager ;



inline void gotoxy(WORD x, WORD y)
{
	COORD pos = { (short)x,(short)y };
	SetConsoleCursorPosition(hCursor, pos);
}

inline void ClearLine(WORD x, WORD y, WORD line)
{
	COORD pos = { (short)x,(short)y };
	SetConsoleCursorPosition(hCursor, pos);
	for (int h = 0; h < line; h++)
	{
		for (int i = 0; i < 10; i++)
		{
			cout << "    ";
		}
		cout << endl;
	}

	SetConsoleCursorPosition(hCursor, pos);
}


inline void CursorView(char show)//Ŀ�������
{

	CONSOLE_CURSOR_INFO ConsoleCursor;

	ConsoleCursor.bVisible = show;
	ConsoleCursor.dwSize = 1;

	SetConsoleCursorInfo(hCursor, &ConsoleCursor);
}

inline COORD getXY() {
	COORD pos;
	CONSOLE_SCREEN_BUFFER_INFO a;

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &a);
	pos.X = a.dwCursorPosition.X;
	pos.Y = a.dwCursorPosition.Y;
	return pos;
}






class Manager
{
public:
	static Manager& CreateInst()
	{
		static Manager* inst_ = new Manager();
		return *inst_;
	}


	static void f_send(SOCKET* m_hSock, char* m_cBuffer, char* m_cMsg, Manager* _manager);
	static void f_recv(SOCKET* m_hSock, char* m_cBuffer, char* m_cMsg, Manager* _manager);


	void ChatSystem(char* Buffer);
	void Print();

	~Manager() {}
	Manager(const Manager&) = delete;  // �̱��� �����������̶�  ����� ���� �������� ����.

protected:

	Manager()
	{
		gotoxy(0, 21);
		for (int i = 0; i < 13; i++)cout << "�����";               //�����ڿ��� ȭ�� ���� �ʱ�ȭ �Ϻ��ϱ� 
	}

private:
	

	string ChatLog[20] ;
	WORD LogSpace = 0;

};




void Manager::ChatSystem(char* Buffer)
{
	if (LogSpace < 20)
	{
		ChatLog[LogSpace] = " ���� : ";
		ChatLog[LogSpace]+=Buffer;
		LogSpace++;
	}
	else if (LogSpace == 20)
	{
		for (int i = 0; i < 19; i++)
		{
			ChatLog[i].clear();
			ChatLog[i] = ChatLog[i + 1];
		}
		ChatLog[19] = " ���� : ";
		ChatLog[19] += Buffer;
	}



}



void Manager::Print()
{
	ClearLine(0, 0, 20);

	for (int i = 0; i < 20; i++)
	{
		cout << ChatLog[i] << endl;
	}



	gotoxy(gPos.X, gPos.Y);
	//Ŀ����ġ�� ä�� ��ġ �� �������� �ٲ۴� . 
	//ä���� ġ���߿� ä���� ���ͼ� ȭ���� ���ŵǸ� Ŀ�� ��ġ�� 
	//���� Ÿ���� �������� ������ ������ ������ �̱���


}




void Manager::f_recv(SOCKET* m_hSock, char* m_cBuffer, char* m_cMsg, Manager* _manager)
{
	while (1)
	{

		recv(*m_hSock, m_cBuffer, PacketSize, 0);
		gPos = getXY();     // ���� Ŀ�� ��ǥ���� ����

		if (SysEnd == 0) { system("cls"); printf(" �ý��� ���� "); break; } // �ý����� ������ ������ ��ɵ� ����

		if (m_cBuffer[0] == '/' && m_cBuffer[1] == 'Q')   // /Q �Է½�  ���� Ŭ���̾�Ʈ ���� .
		{
			send(*m_hSock, m_cBuffer, strlen(m_cMsg), 0);
			SysEnd = 0;
			break;
		}
		else if (m_cBuffer != "")
		{
			_manager->ChatSystem(m_cBuffer);
			_manager->Print();
			memset(m_cBuffer, 0, (size_t)PacketSize);
		}




	}
	return;
}


void Manager::f_send(SOCKET* m_hSock, char* m_cBuffer, char* m_cMsg, Manager* _manager)
{
	while (1)
	{

		//ä�ñ���� �ٽ� 
		ClearLine(0, ChatLine, 1);
		printf(" �Է� : ");

		//scanf("%s \n", cMsg);     
		//scanf�� �����Է��Ŀ� �ѹ��� �����Է��ؾ��� 

		cin >> m_cMsg;


		if (SysEnd == 0) { system("cls"); printf(" ���� ���� "); break; } // �ý����� ������ ������ ��ɵ� ����

		if (m_cMsg[0] == '/' && m_cMsg[1] == 'Q')   // /Q �Է½�  ���� Ŭ���̾�Ʈ ���� .
		{
			send(*m_hSock, m_cMsg, strlen(m_cMsg), 0);
			SysEnd = 0;
			system("cls");
			printf(" �ý��� ���� ");
			break;
		}

		send(*m_hSock, m_cMsg, strlen(m_cMsg), 0);



	}
	return;
}







int main()
{
	cout << "���� �ּҸ� �Է��ϼ��� : ";
	cin >> IPv4;
	system("cls");

	Manager* manager = nullptr;
	manager = &(Manager::CreateInst());



	//Ŀ�� ���ֱ�
	CursorView(0);






	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET hSocket;
	hSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN tAddr = {};
	tAddr.sin_family = AF_INET;
	tAddr.sin_port = htons(Port);
	tAddr.sin_addr.s_addr = inet_addr(IPv4);

	connect(hSocket, (SOCKADDR*)&tAddr, sizeof(tAddr));

	// ���ۿ�  �޼��� ���� 

	char cMsg[] = "Client Send";


	char cBuffer[PacketSize] = {};

	thread Recv(Manager::f_recv, &hSocket, cBuffer, cMsg, manager);
	thread Send(Manager::f_send, &hSocket, cBuffer, cMsg, manager);
	
	

	Recv.join();
	Send.join();

	closesocket(hSocket);

	WSACleanup();

	hCursor = nullptr;   // �ܼ� Ŀ�� �ڵ�  �ΰ����� ����� .
	delete manager;



	return 0;
}



