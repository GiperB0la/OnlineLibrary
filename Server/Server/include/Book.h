#pragma once
#include <string>

class Book
{
public:
	Book(std::string name, std::string author, std::string year) : name(name), author(author), year(year) {};
	~Book() = default;

public:
	std::string name;
	std::string author;
	std::string year;
};