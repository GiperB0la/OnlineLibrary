#include "..\\include\\Library.h"

Library::Library()
	: PATHBOOKS(std::filesystem::current_path()),
	PATHUSERS(std::filesystem::current_path())
{
	std::filesystem::path currentPath = std::filesystem::current_path();

	std::string path_str = currentPath.string();

	size_t pos = path_str.find("build");
	if (pos != std::string::npos) {
		path_str.erase(pos);
	}

	if (!std::filesystem::exists(path_str + "Server\\Data")) {
		if (!std::filesystem::exists(path_str + "Server\\Server\\Data")) {
			if (!std::filesystem::exists(path_str + "Data")) {
				CreateNewData();
				return;
			}
			else {
				path_str += "Data";
			}
		}
		else {
			path_str += "Server\\Server\\Data";
		}
	}
	else
		path_str += "Server\\Data";

	PATHBOOKS = std::filesystem::path(path_str) / "Books";
	PATHUSERS = std::filesystem::path(path_str) / "Users";

	GetDataFromDatabase();
}

void Library::GetDataFromDatabase()
{
	if (!(std::filesystem::is_empty(PATHBOOKS))) {
		for (const auto& file : std::filesystem::directory_iterator(PATHBOOKS)) {
			if (std::filesystem::is_regular_file(file)) {
				std::ifstream inFile(file.path());
				std::string line;
				std::vector<std::string> forBook;
				while (getline(inFile, line)) {
					forBook.push_back(line);
				}
				Book book(forBook.at(0), forBook.at(1), forBook.at(2));
				Books.push_back(book);
			}
		}
	}
}

std::string Library::GetInformationBooks()
{
	std::string message;
	message += "List books in library:\n=====================\n";
	int index = 0;
	for (Book book : Books) {
		message += std::to_string(++index) + ". Book: " + book.name + "\nAuthor: " + book.author + "\nYear: " + book.year + "\n";
		if (index != Books.size())
			message += "\n";
	}
	return message;
}

void Library::CreateNewData()
{
	std::filesystem::create_directories(std::filesystem::current_path() / "Data");
	std::filesystem::create_directories(std::filesystem::current_path() / "Data" / "Books");
	std::filesystem::create_directories(std::filesystem::current_path() / "Data" / "Users");
	
	if (!std::filesystem::exists(std::filesystem::current_path() / "Data")) {
		std::cout << "Error create folder> " << std::filesystem::current_path() << std::endl;
		exit(0);
	}

	PATHBOOKS = std::filesystem::current_path() / "Data" / "Books";
	PATHUSERS = std::filesystem::current_path() / "Data" / "Users";

	CreateNewBooks();
	GetDataFromDatabase();
	CreateFilesBook();
}

void Library::CreateNewBooks()
{
	Books.push_back(std::move(Book("451 degrees Fahrenheit", "Ray Bradbury", "1953")));
	Books.push_back(std::move(Book("A hundred years of loneliness", "Gabriel Garcia Marqu ez", "1967")));
	Books.push_back(std::move(Book("Fight Club", "Chuck Palahniuk", "1996")));
	Books.push_back(std::move(Book("Martin Eden", "Jack London", "1909")));
	Books.push_back(std::move(Book("The hero of our time", "Mikhail Lermontov", "1840")));
}

void Library::CreateFilesBook()
{
	for (Book book : Books) {
		std::ofstream file(PATHBOOKS / (book.name + ".txt"));
		file << book.name << std::endl;
        file << book.author << std::endl;
        file << book.year << std::endl;
	}
}