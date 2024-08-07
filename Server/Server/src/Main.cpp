#include "..\\include\\Library.h"
#include "..\\include\\Server.h"


int main()
{
	Server server(54000, "192.168.1.14");
	server.start();

	return 0;
}