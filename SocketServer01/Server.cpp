#include <stdio.h>
#include <WinSock2.h>
#include <iostream>
#include <thread>


#include <string>
#include <windows.h>



#pragma comment(lib,"ws2_32")





using namespace std;

//전역변수 
static int SysEnd = 1;  // 채팅기능 온오프 표시 1은 on 0은 off


const int PacketSize = 1024;  // 상수 패킷사이즈 
const int Port = 4579;        // 상수 포트 번호 

const UINT ChatLine = 22;     // 채팅이 나올 줄 

static HANDLE hCursor = GetStdHandle(STD_OUTPUT_HANDLE); // 콘솔 커서에 대한 핸들 

COORD gPos = { 0,0 };

//클래스 전방 선언 

class Manager;


inline void gotoxy(WORD x, WORD y)
{
	COORD pos = { (short)x,(short)y };
	SetConsoleCursorPosition(hCursor, pos);
}


inline void ClearLine(WORD x, WORD y,WORD line)
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

inline void CursorView(char show)//커서숨기기
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

	static void f_send(SOCKET* m_hClnt, char* m_cBuffer, char* m_cMsg, Manager* _manager);
	static void f_recv(SOCKET* m_hClnt, char* m_cBuffer, char* m_cMsg, Manager* _manager);

	void ChatSystem(char* Buffer);
	void Print();

	~Manager() {}
	Manager(const Manager&) = delete;  // 싱글톤 디자인패턴이라서  사용할 일은 없으리라 생각.


protected:

	Manager()
	{
		gotoxy(0, 21);
		for (int i = 0; i < 13; i++)cout << "〓〓〓〓";					    //생성자에서 화면 설정 초기화 일부하기 
	}

private:

	


	string ChatLog[20] ;
	WORD LogSpace = 0;
};





void Manager::ChatSystem(char* Buffer)
{
	if (LogSpace < 20)
	{
		ChatLog[LogSpace] = " 클라이언트 : ";
		ChatLog[LogSpace] += Buffer;
		LogSpace++;
	}
	else if (LogSpace == 20)
	{
		for (int i = 0; i < 19; i++)
		{
			ChatLog[i].clear();
			ChatLog[i] = ChatLog[i + 1];
		}
		ChatLog[19] = " 클라이언트 : ";
		ChatLog[19] += (Buffer);
	}
	
}

void Manager::Print()
{
	ClearLine(0, 0, 20);

	for (int i = 0; i < 20; i++)
	{
		cout << ChatLog[i] << endl;
	}
	

	gotoxy(gPos.X,gPos.Y);  
	//커서위치를 채팅 위치 맨 왼쪽으로 바꾼다 . 
	//채팅을 치던중에 채팅이 들어와서 화면이 갱신되면 커서 위치를 
	//원래 타이핑 지점으로 돌려야 하지만 아직은 미구현


}





void Manager::f_recv(SOCKET* m_hClnt, char* m_cBuffer, char* m_cMsg, Manager* _manager)
{


	while (1)
	{
		recv(*m_hClnt, m_cBuffer, PacketSize, 0);
		gPos = getXY();     // 현재 커서 좌표값을 저장

		if (SysEnd == 0) { system("cls"); printf(" 시스템 종료 "); break; } // 시스템이 꺼지면 보내는 기능도 정지

		if (m_cBuffer[0] == '/' && m_cBuffer[1] == 'Q')   // /Q 입력시  서버 클라이언트 종료 .
		{
			send(*m_hClnt, m_cBuffer, strlen(m_cMsg), 0);
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

void Manager::f_send(SOCKET* m_hClnt, char* m_cBuffer, char* m_cMsg, Manager* _manager)
{
	while (1)
	{
		ClearLine(0, ChatLine, 1);
		printf(" 입력 : ");

		cin >> m_cMsg;

		if (SysEnd == 0) { system("cls"); printf(" 서비스  종료 "); break; } // 시스템이 꺼지면 보내는 기능도 정지


		if (m_cMsg[0] == '/' && m_cMsg[1] == 'Q')   // /Q 입력시  서버 클라이언트 종료 .
		{
			send(*m_hClnt, m_cMsg, strlen(m_cMsg), 0);
			SysEnd = 0;
			system("cls");
			printf(" 시스템 종료 ");
			break;
		}


		send(*m_hClnt, m_cMsg, strlen(m_cMsg), 0);

	}
	return;
}







int main()
{

	
	// 매니저 클래스 객체 생성 

	Manager* manager = nullptr;
	manager = &(Manager::CreateInst());


	//커서 없애기
	CursorView(0);



	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET hListen;
	hListen = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN tListenAddr = {};
	tListenAddr.sin_family = AF_INET;
	tListenAddr.sin_port = htons(Port);
	tListenAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(hListen, (SOCKADDR*)&tListenAddr, sizeof(tListenAddr));
	listen(hListen, SOMAXCONN);

	SOCKADDR_IN tClntAddr = {};
	int iClntSize = sizeof(tClntAddr);
    SOCKET hClient = accept(hListen, (SOCKADDR*)&tClntAddr, &iClntSize);





	// 버퍼와 메세지 선언 

	char cBuffer[PacketSize] = {"",};

	char cMsg[] = "Server Send";

	thread Recv(Manager::f_recv, &hClient, cBuffer, cMsg, manager);

	thread Send(Manager::f_send, &hClient, cBuffer, cMsg, manager);



	Recv.join();
	Send.join();


	closesocket(hClient);
	closesocket(hListen);

	WSACleanup();

	hCursor = nullptr;   // 콘솔 커서 핸들  널값으로 만들기 .
	delete manager;


	return 0;
}








