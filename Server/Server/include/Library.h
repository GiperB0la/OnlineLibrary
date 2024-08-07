#pragma once
#include "Book.h"
#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>

class Library
{
public:
	Library();
	~Library() = default;

public:
	std::vector<Book> Books;
	void GetDataFromDatabase();
	std::string GetInformationBooks();
	std::filesystem::path PATHBOOKS;
	std::filesystem::path PATHUSERS;
private:
	void CreateNewData();
	void CreateNewBooks();
	void CreateFilesBook();
};