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

#define NPORTS 10

//сокеты что бы хранить несколько соединений
SOCKADDR_IN ports[NPORTS];
int Amount_Ports=0;
SOCKET Connection;
SOCKET Client;

void ClientHandler(int index,char prevmsg[256]){
	char msg[256]="";
	int iResult=1;

	while(1){

		SOCKADDR_IN client_addrr;
		int client_addrr_size= sizeof(client_addrr);

		int bsize=recvfrom(Connection,&msg[0],sizeof(msg)-1,0,(sockaddr *) &client_addrr, &client_addrr_size);
		
		if (bsize==SOCKET_ERROR){
			printf("Recievefrom() error.\n");
		}

		//Определяем IP-клиента и порт 
		HOSTENT *hostent;

		hostent=gethostbyaddr((char *)&client_addrr.sin_addr,4,AF_INET);

        // Добавление завершающего нуля
        msg[bsize]=0;
 
        // Вывод на экран 
		printf("C=>S:%s\n",&msg[0]);

		// посылка датаграммы клиенту
		for (int i=0;i<Amount_Ports;i++){
			//если порт текцщего клиента не совпадает с номером порта в массиве то отправка
			if (ports[i].sin_port!=client_addrr.sin_port){
			sendto(Connection,&msg[0],bsize,0,
				(sockaddr *)&ports[i], sizeof(ports[i]));}

		}
	}
}

//Добавление соединения в массив
int AddPortToArray(SOCKADDR_IN addr){
	int Fl=1;
	for (int i=0;i<Amount_Ports;i++){
		if (ports[i].sin_port==addr.sin_port){
			Fl=0;
		}
	}
	if (Fl==1){
		ports[Amount_Ports]=addr;
		Amount_Ports++;
	}
	return 0;
}

int  main(int argc, char **argv) 
{
	WSADATA wsaData;
	WORD DLLVersion = MAKEWORD(2,1);//ЗАПРАШИВАЕМАЯ ВЕРСИЯ БИБЛИОТЕКИ ВИНСОК
	int iResult;

	printf("UDP server started.\n");

	//запрашивает версия библиотеки и ссылка на структуру всадата
	iResult = WSAStartup(DLLVersion, &wsaData);
    if (iResult != 0) {
        std::cout<<"WSAStartup failed with error: %d\n";
        return 1;
    }


	Connection=socket (AF_INET,SOCK_DGRAM ,IPPROTO_UDP);
	if (Connection==INVALID_SOCKET){
		printf("Socket Error: %d\n",WSAGetLastError());
		WSACleanup();
	}
	else{
		printf("Socket has been created.\n");
	}

	//Связывание сокета
	SOCKADDR_IN addr;
	int sizeofaddr=sizeof(addr);

	addr.sin_addr.s_addr = inet_addr("127.0.0.1");  //inet_addr("127.0.0.1")
	addr.sin_port = htons(1111);                    //htons(1111)
	addr.sin_family=AF_INET;

	//ПРИВЯЗКА АДРЕСА К СОКЕТУ
	if(bind(Connection,(SOCKADDR*)&addr,sizeof(addr))== SOCKET_ERROR){
		printf("Bind error.\n");
		closesocket(Connection);
	}
	else{
		printf("Socket was binded.\n");
	}


	iResult=1;

	char msg[256]="";

	//Обработка присланных пакетов
	while(Amount_Ports<10){

		SOCKADDR_IN client_addr;
		int client_addr_size= sizeof(client_addr);

		int bsize=recvfrom(Connection,&msg[0],sizeof(msg)-1,0,(sockaddr *) &client_addr, &client_addr_size);
		
		if (bsize==SOCKET_ERROR){
			printf("Recievefrom() error.\n");
		}
		// Добавление завершающего нуля
        msg[bsize]=0;
		
		//Определяем IP-клиента и порт 
		HOSTENT *hostent;

		hostent=gethostbyaddr((char *)&client_addr.sin_addr,4,AF_INET);
		if (hostent!= NULL){

			//добавляю порт в массив только если он новый
			AddPortToArray(client_addr);

			//printf("Amount Ports:%d\n",Amount_Ports);
			//printf("accepted connection from %s, port %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

			for (int i=0;i<Amount_Ports;i++){
				//если порт текцщего клиента не совпадает с номером порта в массиве то отправка
				if (ports[i].sin_port!=client_addr.sin_port){
					sendto(Connection,&msg[0],sizeof(msg)-1,0,
					(sockaddr *)&ports[i], sizeof(ports[i]));}

			}
		}

		CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)ClientHandler,(LPVOID)(Amount_Ports,msg[256]),NULL,NULL);
	}

	WSACleanup();
	return 0;

}
