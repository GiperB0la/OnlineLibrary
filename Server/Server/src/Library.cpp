#include "..\\include\\Library.h"

Library::Library()
{
    std::filesystem::path currentPath = std::filesystem::current_path();
    std::filesystem::path basePath;

    if (std::filesystem::exists(currentPath / "Server" / "Data")) {
        basePath = currentPath / "Server" / "Data";
    }
    else if (std::filesystem::exists(currentPath / "Server" / "Server" / "Data")) {
        basePath = currentPath / "Server" / "Server" / "Data";
    }
    else if (std::filesystem::exists(currentPath / "Data")) {
        basePath = currentPath / "Data";
    }
    else {
        CreateNewData();
        return;
    }

    PATHBOOKS = basePath / "Books";
    PATHUSERS = basePath / "Users";

    GetDataFromDatabase();
}

void Library::GetDataFromDatabase()
{
    if (!Books.empty() || std::filesystem::is_empty(PATHBOOKS)) return;

    for (const auto& file : std::filesystem::directory_iterator(PATHBOOKS)) {
        if (std::filesystem::is_regular_file(file)) {
            std::ifstream inFile(file.path());
            std::vector<std::string> bookDetails;
            std::string line;

            while (getline(inFile, line)) {
                bookDetails.push_back(line);
            }

            if (bookDetails.size() >= 3) {
                Books.emplace_back(bookDetails[0], bookDetails[1], bookDetails[2]);
            }
        }
    }
}

std::string Library::GetInformationBooks()
{
    std::string message = "List of books in library:\n=====================\n";
    int index = 0;

    for (const auto& book : Books) {
        message += std::to_string(++index) + ". Book: " + book.name + "\nAuthor: " + book.author + "\nYear: " + book.year + "\n";
        if (index != Books.size())
            message += "\n";
    }

    return message;
}

void Library::CreateNewData()
{
    std::filesystem::path dataPath = std::filesystem::current_path() / "Data";

    std::filesystem::create_directories(dataPath / "Books");
    std::filesystem::create_directories(dataPath / "Users");

    PATHBOOKS = dataPath / "Books";
    PATHUSERS = dataPath / "Users";

    CreateNewBooks();
    CreateFilesBook();
}

void Library::CreateNewBooks()
{
    Books.emplace_back("451 degrees Fahrenheit", "Ray Bradbury", "1953");
    Books.emplace_back("A Hundred Years of Solitude", "Gabriel Garcia Marquez", "1967");
    Books.emplace_back("Fight Club", "Chuck Palahniuk", "1996");
    Books.emplace_back("Martin Eden", "Jack London", "1909");
    Books.emplace_back("The Hero of Our Time", "Mikhail Lermontov", "1840");
}

void Library::CreateFilesBook()
{
    for (const auto& book : Books) {
        std::ofstream file(PATHBOOKS / (book.name + ".txt"));
        file << book.name << std::endl;
        file << book.author << std::endl;
        file << book.year << std::endl;
    }
}
