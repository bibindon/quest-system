#pragma comment(lib, "QuestSystem.lib")

#include <iostream>
#include "..\QuestSystem\QuestSystem.h"

using namespace NSQuestSystem;

int main()
{
    QuestSystem qs;
    bool ret = qs.Init("sample.csv", "", false);
    qs.SetTalk("‚«‚ñ‚É‚­‚ñ");
    std::vector<std::string> startedQuest = qs.GetStartQuest();
    std::cout << startedQuest.at(0) << std::endl; // "Q1"

    std::vector<std::string> startEvent = qs.GetQuestStartEvent("Q1");
    std::cout << startEvent.at(0) << std::endl; // "<speak><‚«‚ñ‚É‚­‚ñ>ƒ]ƒ“ƒr‚ð‚R•C“|‚µ‚Ä‚­‚ê"

    qs.SetDefeatEnemy("ƒXƒ‰ƒCƒ€");
    qs.SetDefeatEnemy("ƒXƒ‰ƒCƒ€");
    qs.SetDefeatEnemy("ƒXƒ‰ƒCƒ€");
    qs.SetTalk("‚«‚ñ‚É‚­‚ñ");

    std::vector<std::string> finishQuest = qs.GetFinishQuest();
    std::cout << finishQuest.at(0) << std::endl; // "Q1"

    std::vector<std::string> finishEvent = qs.GetQuestFinishEvent("Q1");
    std::cout << finishEvent.at(0) << std::endl; // "<speak><‚«‚ñ‚É‚­‚ñ>ƒ]ƒ“ƒr‚ð‚R•C“|‚µ‚Ä‚­‚ê"

    return 0;
}

