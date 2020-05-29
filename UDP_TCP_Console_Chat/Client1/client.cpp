#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#pragma comment (lib, "Ws2_32.lib")


SOCKET Connection;

//принятие от сервера сообщение
void ClientHandler(){
	char msg[256];
	int iResult=1;

	while(iResult>0){
		iResult=recv(Connection,msg,sizeof(msg),NULL);
		std::cout<<msg<<std::endl;
	}
}

int  main(int argc, char **argv) 
{
	WSADATA wsaData;
	WORD DLLVersion = MAKEWORD(2,1);//ЗАПРАШИВАЕМАЯ ВЕРСИЯ БИБЛИОТЕКИ ВИНСОК
	int iResult;

	//запрашивает версия библиотеки и ссылка на структуру всадата
	iResult = WSAStartup(DLLVersion, &wsaData);
    if (iResult != 0) {
        std::cout<<"WSAStartup failed with error: %d\n";
        return 1;
    }
 

	SOCKADDR_IN addr;
	int sizeofaddr=sizeof(addr);
	addr.sin_addr.s_addr  = inet_addr("127.0.0.1");
	addr.sin_port = htons(1111);
	addr.sin_family=AF_INET;

	Connection = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

	if (connect(Connection,(SOCKADDR*)&addr,sizeof(addr))!=0){
		std::cout<<"no\n";
		return 1;

	}

	std::cout<<"connected...";

	//4 парам нул ибо нет параметров функции запуск функции в новом потоке
	HANDLE thread=CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)ClientHandler,NULL,NULL,NULL);

	char msg1[256];
	//бесконечный цикл
	while(true){
		std::cin.getline(msg1,sizeof(msg1));
		send(Connection,msg1,sizeof(msg1),NULL);
		Sleep(10);
	}
	
	closesocket(Connection);
	CloseHandle(thread);

	WSACleanup();
	system("pause");
	return 0;

}