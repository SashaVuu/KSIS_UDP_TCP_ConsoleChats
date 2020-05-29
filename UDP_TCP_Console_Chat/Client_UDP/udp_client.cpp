#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#pragma comment (lib, "Ws2_32.lib")

SOCKET My_Sock;
SOCKADDR_IN dest_addr;

//�������� �� ������� ���������
void ClientHandler(){
	char msg[256]="";
	int bsize=1;
	// ����� ��������� � �������
	int dest_addr_size=sizeof(dest_addr);

	while(bsize>0){

		bsize=recvfrom(My_Sock,&msg[0],sizeof(msg)-1,0, (sockaddr *) &dest_addr, &dest_addr_size);
 
		if (bsize==SOCKET_ERROR)
		{
		  printf("recvfrom() error:"\
			"%d\n",WSAGetLastError());
		  closesocket(My_Sock);
		  WSACleanup();
		}

		msg[bsize]=0;

		// ����� ��������� � ������� ��������� �� �����
		printf("%s",&msg[0]);
	}
}


int  main(int argc, char **argv) 
{
	WSADATA wsaData;
	WORD DLLVersion = MAKEWORD(2,1);//������������� ������ ���������� ������
	int iResult;

	printf("UDP Client Started");

	//����������� ������ ���������� � ������ �� ��������� �������
	iResult = WSAStartup(DLLVersion, &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error\n");
        return 1;
    }

	//�������� ������
	//SOCKET My_Sock;
	My_Sock = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	if (My_Sock==INVALID_SOCKET){
		printf("Socket Error: %d\n",WSAGetLastError());
		WSACleanup();
	}
	else{
		printf("Socket has been created.\n");
	}

	// ����� ��������� � ��������
    HOSTENT *hostent;

	//SOCKADDR_IN dest_addr;
	dest_addr.sin_addr.s_addr  = inet_addr("127.0.0.1");
	dest_addr.sin_port = htons(1111);
	dest_addr.sin_family=AF_INET;

	bind(My_Sock,(SOCKADDR*)&dest_addr,sizeof(dest_addr));


	char msg[256]="New client here!\n";

	// �������� ��������� �� ������
    sendto(My_Sock,&msg[0],strlen(&msg[0]),0, (sockaddr *) &dest_addr,sizeof(dest_addr));

	HANDLE thread=CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)ClientHandler,NULL,NULL,NULL);


	while(1)
    {
      // ������ ��������� � ����������
      //printf("\n");
	  fgets(&msg[0],sizeof(msg)-1,stdin);
      if (!strcmp(&msg[0],"quit\n")) break;
 
      // �������� ��������� �� ������
      sendto(My_Sock,&msg[0],strlen(&msg[0]),0, (sockaddr *) &dest_addr,sizeof(dest_addr));
 
    }
 
	closesocket(My_Sock);
    CloseHandle(thread);

	WSACleanup();
	system("pause");
	return 0;

}