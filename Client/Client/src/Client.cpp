#include "..\\include\\Client.h"

Client::Client(int port, std::string ipAddress) : wsa{ 0 }, port(port), ipAddress(ipAddress),
clientSocket(INVALID_SOCKET), info{ 0 }, infoLength(sizeof(info)), recvLength(0), registration(false) {}

Client::~Client() {
    closesocket(clientSocket);
    WSACleanup();
}

void Client::init()
{
    info.sin_family = AF_INET;
    info.sin_port = htons(port);
    info.sin_addr.s_addr = inet_addr(ipAddress.c_str());

    int wsaStartupResult = WSAStartup(MAKEWORD(2, 2), &wsa);
    if (wsaStartupResult != 0) {
        std::cerr << "[-] WSAStartup failed with error: " << wsaStartupResult << std::endl;
        exit(0);
    }
    std::cout << "[+] WSA success" << std::endl;

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "[-] Couldn't create socket, error: " << WSAGetLastError() << std::endl;
        exit(0);
    }
    std::cout << "[+] Socket success" << std::endl;

    if (connect(clientSocket, (SOCKADDR*)&info, infoLength) == SOCKET_ERROR) {
        std::cerr << "[-] Connect failed with error: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        exit(0);
    }
    std::cout << "Client success connecting" << std::endl;
}

void Client::start()
{
    init();
    system("cls");
    mainMenu();

    while (true) {
        sendM();
        receive();
    }
}

void Client::receive()
{
    char buffer[SIZE];
    memset(buffer, 0, SIZE);
    int recvLength = recv(clientSocket, buffer, SIZE, 0);
    if (recvLength > 0) {
        system("cls");
        message = buffer;
        if (message == "Error register") {
            registration = false;
            std::cout << "There is no such account." << std::endl;
            std::cout << "Would you like to register?\n\t<Yes/No>" << std::endl << std::endl;
            login.clear();
        }
        else if (message == "log" && !registration) {
            system("cls");
            std::cout << "Enter your login to log in:" << std::endl << std::endl;
            registration = true;
        }
        else if (message == "Yes" || message == "yes" || message == "YES") {
            system("cls");
            std::cout << "Enter your login to register:" << std::endl << std::endl;
            registration = true;
        }
        else if (message == "No" || message == "no" || message == "NO") {
            mainMenu();
        }
        else if (message == "clear") {
            system("cls");
            if (!registration)
                mainMenu();
            else
                secondMenu();
        }
        else if (message == "exit") {
            if (registration) {
                registration = !registration;
                mainMenu();
                login = "";
            }
        }
        else if ((message == "Successfully") || (message.at(0) == 'R' && message.at(1) == 'e' && message.at(2) == 'g')) {
            system("cls");
            secondMenu();
        }
        else {
            std::cout << message << std::endl;
        }
    }
    else if (recvLength == 0) {
        std::cout << "Server closed connection." << std::endl;
        exit(0);
    }
    else {
        std::cerr << "[-] Receive failed with error: " << WSAGetLastError() << std::endl;
    }
}

void Client::sendM()
{
    std::cout << "> ";
    std::getline(std::cin, message);
    if (registration && login == "")
        login = message;
    if (send(clientSocket, message.c_str(), message.size(), 0) == SOCKET_ERROR) {
        std::cerr << "[-] Send failed with error: " << WSAGetLastError() << std::endl;
    }
}

void Client::stop()
{
    closesocket(clientSocket);
    WSACleanup();
}

void Client::mainMenu()
{
    std::cout << "Welcome to the library!" << std::endl;
    std::cout << "'list' - List of books in the library()" << std::endl;
    std::cout << "'log' - Log in to the system()" << std::endl;
    std::cout << "'clear' - Clear()" << std::endl;
    std::cout << "'exit' - Exit()" << std::endl << std::endl;
}

void Client::secondMenu()
{
    std::cout << "Hello, " << login << "." << std::endl;
    std::cout << "'list' - List of books in the library()" << std::endl;
    std::cout << "'books' - My list of books()" << std::endl;
    std::cout << "'take' - Take a book()" << std::endl;
    std::cout << "'return' - Return of books()" << std::endl;
    std::cout << "'clear' - Clear()" << std::endl;
    std::cout << "'exit' - Exit the menu()" << std::endl << std::endl;
}