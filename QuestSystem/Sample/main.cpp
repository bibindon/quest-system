#pragma comment(lib, "QuestSystem.lib")

#include <iostream>
#include "..\QuestSystem\QuestSystem.h"

#include <tchar.h>

using namespace NSQuestSystem;

int main()
{
    QuestSystem qs;
    bool ret = qs.Init(_T("sample.csv"), _T(""), false);
    qs.SetTalk(_T("きんにくん"));
    std::vector<std::wstring> startedQuest = qs.GetStartQuest();
    std::wcout << startedQuest.at(0) << std::endl; // "Q1"

    std::vector<std::wstring> startEvent = qs.GetQuestStartEvent(_T("Q1"));
    std::wcout << startEvent.at(0) << std::endl; // "<speak><きんにくん>ゾンビを３匹倒してくれ"

    qs.SetDefeatEnemy(_T("スライム"));
    qs.SetDefeatEnemy(_T("スライム"));
    qs.SetDefeatEnemy(_T("スライム"));
    qs.SetTalk(_T("きんにくん"));

    std::vector<std::wstring> finishQuest = qs.GetFinishQuest();
    std::wcout << finishQuest.at(0) << std::endl; // "Q1"

    std::vector<std::wstring> finishEvent = qs.GetQuestFinishEvent(_T("Q1"));
    std::wcout << finishEvent.at(0) << std::endl; // "<speak><きんにくん>ゾンビを３匹倒してくれ"

    return 0;
}

