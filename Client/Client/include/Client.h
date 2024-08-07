#pragma once
#include <string>
#include <iostream>
#include <assert.h>
#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")
#pragma warning(disable: 4996)

#define SIZE 1024

class Client
{
public:
	Client(int port, std::string ipAddress);
	~Client();

private:
	WSADATA wsa;
	SOCKET clientSocket;
	std::string ipAddress;
	int port;
	char buffer[SIZE];
	sockaddr_in info;
	std::string message;
	int infoLength;
	int recvLength;

	std::string login;
	bool registration;

public:
	void start();

private:
	void init();
	void stop();
	void receive();
	void sendM();
	void mainMenu();
	void secondMenu();
};