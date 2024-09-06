
#pragma comment(lib, "QuestSystem.lib")

#include <iostream>
#include "QuestSystem.h"

int main()
{
	QuestSystem qs;
	qs.Init("hogehoge");
	std::cout << "sample" << std::endl;
	return 0;
}
