#undef UNICODE
#define WIN32_LEAN_AND_MEAN
// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
//#pragma comment (lib, "Mswsock.lib")

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string.h>

//������ ��� �� ������� ��������� ����������
SOCKET Connections[100];
int Counter=0;//��������� ��� �������� ������� ����������

void ClientHandler(int index){
	char msg[256]="";
	int iResult=1;

	while(iResult>0){
		//����� ��������� ������������
		iResult=recv(Connections[index],msg,sizeof(msg),NULL);
		if (iResult>0){
			printf("Get message : %s\n",msg);
		}
		else
		{
			printf("Error with recievieng\n");
		}

		//�������� ���� ����� �����������
		for(int i=0;i<Counter;i++){
			if (i!=index){
				if (send(Connections[i],msg,sizeof(msg),NULL)>0){
					printf("Send to everyone message : %s\n",msg);
				}
				else
				{
					printf("Error with sending\n");
				}
			}
		}
	}
}

int  main(int argc, char **argv) 
{
	
	WSADATA wsaData;
	WORD DLLVersion = MAKEWORD(2,1);//������������� ������ ���������� ������
	int iResult;

	iResult = WSAStartup(DLLVersion, &wsaData);
    if (iResult != 0) {
        std::cout<<"WSAStartup failed with error: %d\n";
        return 1;
    }



	SOCKADDR_IN addr;
	int sizeofaddr=sizeof(addr);

	addr.sin_addr.s_addr = inet_addr("127.0.0.1");  //inet_addr("127.0.0.1")
	addr.sin_port = htons(1111);         //htons(1111)
	addr.sin_family=AF_INET;


	//��� �� ���������� ���������� ���� �� ������ ������ �������� ������������� �� �����
	SOCKET sListen= socket (AF_INET,SOCK_STREAM,IPPROTO_TCP);

	//�������� ������ � ������
	bind(sListen,(SOCKADDR*)&addr,sizeof(addr));

	//������������� ������ 
	listen(sListen,SOMAXCONN);

	//����� ����� ��� �� ����������� ���������� � ��������
	SOCKET newConnection;

	
	for(int i=0;i<100;i++){

		/*��������� �������� ������ �� ��������� ����������, ���������
		������������� � ����������� �������� ������������� ������������.
		������� Accept � �������� ������ ������ � ����������
		����������, � ������ �������� ����� �������� ������� Send � Recv.*/
		newConnection=accept(sListen,(SOCKADDR*)&addr,&sizeofaddr);

		if (newConnection==0){
			std::cout<<"WSAStartup failed with error: %d\n";
		}
		else
		{	
			char msg[256]="Hello user!\n";
			send(newConnection,msg,sizeof(msg),NULL);

			Connections[i]=newConnection;
			Counter++;

			CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)ClientHandler,(LPVOID)(i),NULL,NULL);
		}
		
	}

	WSACleanup();
	return 0;

}
