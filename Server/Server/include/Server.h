#pragma once
#include <string>
#include <vector>
#include <thread>
#include <iostream>
#include <assert.h>
#include <WinSock2.h>
#include <cstdio>
#pragma comment(lib, "Ws2_32.lib")
#pragma warning(disable: 4996)

#define SIZE 1024

#include "Library.h"

class Server;
class ClientSocket
{
public:
	friend Server;
	ClientSocket(SOCKET clientSocket, sockaddr_in clientInfo, int clientInfoLength)
		: clientSocket(clientSocket), clientInfo(clientInfo), clientInfoLength(clientInfoLength), recvLength(0) {};
	~ClientSocket() = default;

private:
	SOCKET clientSocket;
	sockaddr_in clientInfo;
	std::string message;
	int clientInfoLength;
	int recvLength;
};

class Server
{
public:
	Server(int port, std::string ipAddress);
	~Server();

private:
	WSADATA wsa;
	SOCKET serverSocket;
	std::string ipAddress;
	std::string message;
	int port;
	sockaddr_in info;
	int infoLength;
	std::vector<ClientSocket> Clients;

	Library library;

public:
	void start();
	void stop();

private:
	void init();
	void handleClient(ClientSocket Client);
	void connecting();
	bool authorization(bool& reg, std::string& login);
	void registration(std::string& login);
	void listBookUser(std::vector<Book>& UserBooks);
	void GetDataFromDatabaseBooksUser(std::vector<Book>& UserBooks, const std::string login);
	void TakeBook();
	void TakeBookFromBase(std::string& login, std::vector<Book>& UserBooks);
	void ReturnBook(std::vector<Book>& UserBooks);
	void ReturnBookFromBase(std::string& login, std::vector<Book>& UserBooks);
};