#include "..\\include\\Client.h"

int main()
{
	Client client(54000, "192.168.1.14");
	client.start();

	return 0;
}