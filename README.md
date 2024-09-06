# quest-system

こんな感じのCSVファイルを用意して
![image](https://github.com/user-attachments/assets/1d9f1ca6-4bbf-4a0c-bb3b-191878336963)


こんな風に使う。

```cpp
#pragma comment(lib, "QuestSystem.lib")

#include <iostream>
#include "QuestSystem.h"

int main()
{
    QuestSystem qs;
    bool ret = qs.Init("sample.csv");
    qs.SetTalk("きんにくん");
    std::vector<std::string> startedQuest = qs.GetStartQuest();
    std::cout << startedQuest.at(0) << std::endl; // "Q1"

    std::vector<std::string> startEvent = qs.GetQuestStartEvent("Q1");
    std::cout << startEvent.at(0) << std::endl; // "<speak><きんにくん>ゾンビを３匹倒してくれ"

    qs.SetDefeatEnemy("スライム");
    qs.SetDefeatEnemy("スライム");
    qs.SetDefeatEnemy("スライム");
    qs.SetTalk("きんにくん");

    std::vector<std::string> finishQuest = qs.GetFinishQuest();
    std::cout << finishQuest.at(0) << std::endl; // "Q1"

    std::vector<std::string> finishEvent = qs.GetQuestFinishEvent("Q1");
    std::cout << finishEvent.at(0) << std::endl; // "<speak><きんにくん>ありがとうございました"
    std::cout << finishEvent.at(1) << std::endl; // "<image>kinniku.png"

    return 0;
}

```

- コールバック関数を使わないでやってみる予定。
- 「受託したら即完了となるクエスト」と見なすことでNPCのセリフも設定できるのではないかとお試し中。

