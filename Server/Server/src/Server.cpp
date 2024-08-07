#include "..\\include\\Server.h"

Server::Server(int port, std::string ipAddress) : wsa{ 0 }, port(port), ipAddress(ipAddress),
serverSocket(INVALID_SOCKET), info{ 0 }, infoLength(sizeof(info)) {}

Server::~Server() {
    closesocket(serverSocket);
    WSACleanup();
}

void Server::init()
{
    info.sin_family = AF_INET;
    info.sin_port = htons(port);
    info.sin_addr.s_addr = inet_addr(ipAddress.c_str());

    int wsaStartupResult = WSAStartup(MAKEWORD(2, 2), &wsa);
    if (wsaStartupResult != 0) {
        std::cout << "[-] WSAStartup failed with error: " << wsaStartupResult << std::endl;
        return;
    }
    std::cout << "[+] WSA success" << std::endl;

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cout << "[-] Couldn't create socket, error: " << WSAGetLastError() << std::endl;
        return;
    }
    std::cout << "[+] Socket success" << std::endl;

    int bindResult = bind(serverSocket, (sockaddr*)&info, infoLength);
    if (bindResult == SOCKET_ERROR) {
        std::cout << "[-] Couldn't bind socket, error: " << WSAGetLastError() << std::endl;
        return;
    }
    std::cout << "[+] Bind success" << std::endl;
}

void Server::start()
{
    init();

    std::thread connectingThread(&Server::connecting, this);
    connectingThread.detach();
    std::string command;
    Sleep(100);

    while (true) {
        std::getline(std::cin, command);
        int num = 0;

        if (command == "clear") {
            system("cls");
        }
        else if (command == "clients") {
            if (Clients.empty())
                std::cout << "The list of clients is empty." << std::endl;
            for (ClientSocket Client : Clients) {
                std::cout << ++num << ". " << inet_ntoa(Client.clientInfo.sin_addr) << ":" << ntohs(Client.clientInfo.sin_port) << std::endl;
            }
            std::cout << std::endl;
        }
    }
}

void Server::connecting()
{
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cout << "[-] Listen failed with error: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return;
    }
    std::cout << "[+] Listening for connections..." << std::endl;

    while (true) {
        sockaddr_in clientInfo;
        int clientInfoLength = sizeof(clientInfo);
        SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientInfo, &clientInfoLength);
        if (clientSocket == INVALID_SOCKET) {
            std::cout << "[-] Accept failed with error: " << WSAGetLastError() << std::endl;
            continue;
        }
        ClientSocket Client(clientSocket, clientInfo, clientInfoLength);
        std::cout << "Client connected from: " << inet_ntoa(Client.clientInfo.sin_addr) << ":" << ntohs(Client.clientInfo.sin_port) << std::endl;

        std::thread clientThread(&Server::handleClient, this, Client);
        clientThread.detach();
    }
}

void Server::handleClient(ClientSocket Client)
{
    Clients.push_back(Client);

    char buffer[SIZE];
    bool reg = false;
    bool tBook = false;
    bool rBook = false;
    std::string login;
    std::vector<Book> UserBooks;
    while (true) {
        memset(buffer, 0, SIZE);
        int recvLength = recv(Client.clientSocket, buffer, SIZE, 0);
        if (recvLength == SOCKET_ERROR || recvLength == 0) {
            closesocket(Client.clientSocket);
            for (size_t i = 0; i < Clients.size(); i++) {
                if (Clients.at(i).clientSocket == Client.clientSocket) {
                    std::cout << "Client " << i + 1 << " " << inet_ntoa(Client.clientInfo.sin_addr) << ":" << ntohs(Client.clientInfo.sin_port) << " disconnected." << std::endl;
                    Clients.erase(Clients.begin() + i);
                }
            }
            return;
        }

        message = buffer;
        if (message == "list") {
            message = library.GetInformationBooks();
        }
        else if (message == "log" && !reg) {
            reg = true;
        }
        else if (message == "Yes" || message == "yes" || message == "YES") {
            reg = true;
        }
        else if (message == "No" || message == "no" || message == "NO") {
            reg = false;
            login.clear();
        }
        else if (message == "books" && reg) {
            listBookUser(UserBooks);
        }
        else if (message == "take" && reg) {
            TakeBook();
            tBook = true;
        }
        else if (message == "return" && reg) {
            ReturnBook(UserBooks);
            rBook = true;
        }
        else if (message == "clear") {

        }
        else if (message == "exit") {
            if (!reg) {
                closesocket(Client.clientSocket);
            }
            else {
                reg = false;
                login = "";
                UserBooks.clear();
            }
        }
        else {
            if (reg) {
                if (login.empty()) {
                    if (authorization(reg, login))
                        GetDataFromDatabaseBooksUser(UserBooks, login);
                }
                else if (login == "Error register") {
                    login = message;
                    message = "Registration was successful.\nYour username: " + login + "\n";
                    registration(login);
                }
            }
            if (tBook) {
                TakeBookFromBase(login, UserBooks);
                tBook = false;
            }
            if (rBook) {
                ReturnBookFromBase(login, UserBooks);
                rBook = false;
            }
        }

        if (send(Client.clientSocket, message.c_str(), message.size(), 0) == SOCKET_ERROR) {
            for (size_t i = 0; i < Clients.size(); i++) {
                if (Clients.at(i).clientSocket == Client.clientSocket) {
                    std::cout << "Client " << i + 1 << " " << inet_ntoa(Client.clientInfo.sin_addr) << ":" << ntohs(Client.clientInfo.sin_port) << " disconnected." << std::endl;
                    Clients.erase(Clients.begin() + i);
                }
            }
        }
    }
}

bool Server::authorization(bool& reg, std::string& login)
{
    std::filesystem::path Path = library.PATHUSERS / (message);
    if (std::filesystem::exists(Path)) {
        login = message;
        message = "Successfully";
    }
    else {
        reg = false;
        message = "Error register";
        login = message;
    }
    return reg;
}

void Server::registration(std::string& login)
{
    std::filesystem::path Path = library.PATHUSERS / (login);
    if (!std::filesystem::create_directory(Path))
        perror("Folder creation error.");
}

void Server::stop()
{
    closesocket(serverSocket);
    WSACleanup();
}

void Server::listBookUser(std::vector<Book>& UserBooks)
{
    message.clear();
    message += "A list of your books:\n=====================\n";
    int index = 0;
    for (Book book : UserBooks) {
        message += std::to_string(++index) + ". Book: " + book.name + "\nAuthor: " + book.author + "\nYear: " + book.year + "\n";
        if (index != UserBooks.size())
            message += "\n";
    }
}

void Server::GetDataFromDatabaseBooksUser(std::vector<Book>& UserBooks, const std::string login)
{
    std::filesystem::path Path = library.PATHUSERS / (login);
    if (!(std::filesystem::is_empty(Path))) {
        for (const auto& file : std::filesystem::directory_iterator(Path)) {
            if (std::filesystem::is_regular_file(file)) {
                std::ifstream inFile(file.path());
                std::string line;
                std::vector<std::string> forBook;
                while (getline(inFile, line)) {
                    forBook.push_back(line);
                }
                Book book(forBook.at(0), forBook.at(1), forBook.at(2));
                UserBooks.push_back(book);
            }
        }
    }
}

void Server::TakeBook()
{
    message.clear();
    message = "Which book do you want to take?\n";
    int index = 0;
    for (Book book : library.Books) {
        message += std::to_string(++index) + ". Book: " + book.name + "\n";
    }
}

void Server::TakeBookFromBase(std::string& login, std::vector<Book>& UserBooks)
{
    int index = stoi(message);
    message.clear();
    if (index > library.Books.size() || index < 1)
        message = "You entered the wrong book number!\n";
    else if (library.Books.empty())
        message = "There are no books in the library!\n";
    else {
        std::filesystem::path Path = library.PATHUSERS / (login + "\\" + library.Books.at(index - 1).name + ".txt");
        std::ofstream outFile(Path);
        outFile << library.Books.at(index - 1).name << std::endl;
        outFile << library.Books.at(index - 1).author << std::endl;
        outFile << library.Books.at(index - 1).year << std::endl;
        message = "Did you take the book: " + library.Books.at(index - 1).name + "." + "\n";
        UserBooks.push_back(library.Books.at(index - 1));
        Path = library.PATHBOOKS / (library.Books.at(index - 1).name + ".txt");
        if (std::filesystem::remove(Path) == 0) {
            perror("File deletion error.");
        }
        library.Books.erase(library.Books.begin() + index - 1);
        listBookUser(UserBooks);
    }
}

void Server::ReturnBook(std::vector<Book>& UserBooks)
{
    message.clear();
    if (UserBooks.size() == 0) {
        message += "You have nothing to return.\n";
    }
    else {
        message += "Which book do you want to return?\n";
        message += "A list of your books:\n=====================\n";
        int index = 0;
        for (Book book : UserBooks) {
            message += std::to_string(++index) + ". Book: " + book.name + "\n";
        }
    }
}

void Server::ReturnBookFromBase(std::string& login, std::vector<Book>& UserBooks)
{
    int index = stoi(message);
    message.clear();
    if (index > UserBooks.size() || index < 1)
        message += "You entered the wrong book number.\n";
    else if (UserBooks.empty())
        message = "You don't have any books.\n";
    else {
        std::filesystem::path Path = library.PATHBOOKS / (UserBooks.at(index - 1).name + ".txt");
        std::ofstream outFile(Path);
        outFile << UserBooks.at(index - 1).name << std::endl;
        outFile << UserBooks.at(index - 1).author << std::endl;
        outFile << UserBooks.at(index - 1).year << std::endl;
        message = "You have chosen a book: " + UserBooks.at(index - 1).name + "." + "\n";
        library.Books.push_back(UserBooks.at(index - 1));
        Path = library.PATHUSERS / (login + "\\" + UserBooks.at(index - 1).name + ".txt");
        if (std::filesystem::remove(Path) == 0) {
            perror("File deletion error.");
        }
        UserBooks.erase(UserBooks.begin() + index - 1);
        listBookUser(UserBooks);
    }
}