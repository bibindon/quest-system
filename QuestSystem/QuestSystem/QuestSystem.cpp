#include "QuestSystem.h"
#include <windows.h>
#include <fstream>
#include <sstream>
#include "Shlwapi.h"

#pragma comment( lib, "Shlwapi.lib" ) 

QuestSystem::QuestSystem()
{
}

void ltrim(std::string& s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
        [](unsigned char ch)
        {
            return !std::isspace(ch);
        }
    ));
}

void rtrim(std::string& s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(),
        [](unsigned char ch)
        {
            return !std::isspace(ch);
        }
    ).base(), s.end());
}

void trim(std::string& s)
{
    rtrim(s);
    ltrim(s);
}

/*

クエストID,開始タイプ,開始タイプオプション,開始イベント,完了タイプ,完了オプション１,完了オプション２,完了イベント
Q1,人と話したら,きんにくん,<speak><きんにくん>ゾンビを３匹倒してくれ,"敵を倒したら
人と話したら","スライム
きんにくん",3,"<speak><きんにくん>ありがとうございました
<image>kinniku.png"
Q2,人と話したら,きんにくん,<speak><きんにくん>ゾンビとスライムを倒してくれ,"敵を倒したら
敵を倒したら
人と話したら","スライム
ゾンビ
きんにくん","3
4","<speak><きんにくん>ありがとうございました
<image>kinniku.png"
Q3,"人と話したら
クエストが完了していないなら","シュワちゃん
Q3",<speak><シュワちゃん>こんにちは,自動完了,,,
Q4,"人と話したら
クエストが完了していたら","シュワちゃん
Q3",<speak><シュワちゃん>なんかようですか,自動完了,,,

*/
bool QuestSystem::Init(const std::string& csvFilePath)
{
    int result = PathFileExists(csvFilePath.c_str());
    if (result == 0)
    {
        return false;
    }

    std::ifstream ifs(csvFilePath);
    std::string buff;
    std::string buffComma;
    QuestData questData;
    int row = 0;
    int col = 0;
    bool doubleQuote = false;
    bool doubleQuoteMode = false;

    while (std::getline(ifs, buff))
    {
        // 先頭行は無視
        if (row == 0)
        {
            row = 1;
            continue;
        }
        std::istringstream iss(buff);

        while (std::getline(iss, buffComma, ','))
        {
            trim(buffComma);
            if (buffComma.find('"') != std::string::npos)
            {
                doubleQuote = true;

                if (buffComma.at(0) == '"')
                {
                    doubleQuoteMode = true;
                }
                else
                {
                    doubleQuoteMode = false;
                }
                buffComma.erase(std::remove(buffComma.begin(), buffComma.end(), '"'), buffComma.end());
            }
            if (col == 0)
            {
                questData.SetId(buffComma);
            }
            else if (col == 1)
            {
                std::vector<eStartType> work = questData.GetStartType();
                std::deque<bool> work2 = questData.GetStartFlag();
                if (buffComma == "人と話したら")
                {
                    work.push_back(eStartType::TALK);
                    work2.push_back(false);
                }
                else if (buffComma == "クエストが完了していたら")
                {
                    work.push_back(eStartType::QUEST_FINISHED);
                    work2.push_back(false);
                }
                else if (buffComma == "クエストが完了していないなら")
                {
                    work.push_back(eStartType::QUEST_NOT_FINISHED);
                    work2.push_back(true);
                }
                questData.SetStartType(work);
                questData.SetStartFlag(work2);
            }
            else if (col == 2)
            {
                std::vector<std::string> work = questData.GetStartOption1();
                work.push_back(buffComma);
                questData.SetStartOption1(work);
            }
            else if (col == 3)
            {
                std::vector<std::string> work = questData.GetStartEvent();
                work.push_back(buffComma);
                questData.SetStartEvent(work);
            }
            else if (col == 4)
            {
                std::vector<eFinishType> work = questData.GetFinishType();
                std::deque<bool> work2 = questData.GetFinishFlag();
                if (buffComma == "敵を倒したら")
                {
                    work.push_back(eFinishType::DEFEAT_ENEMY);
                    work2.push_back(false);
                }
                else if (buffComma == "人と話したら")
                {
                    work.push_back(eFinishType::TALK);
                    work2.push_back(false);
                }
                else if (buffComma == "自動完了")
                {
                    work.push_back(eFinishType::AUTO);
                    work2.push_back(true);
                }
                questData.SetFinishType(work);
                questData.SetFinishFlag(work2);
            }
            else if (col == 5)
            {
                std::vector<std::string> work = questData.GetFinishOption1();
                work.push_back(buffComma);
                questData.SetFinishOption1(work);
            }
            else if (col == 6)
            {
                std::vector<std::string> work = questData.GetFinishOption2();
                work.push_back(buffComma);
                questData.SetFinishOption2(work);

                std::unordered_map<int, int> work2 = questData.GetCurrentFinishOpt2();
                int work3 = 0;
                std::stringstream(buffComma) >> work3;
                work2[work.size()-1] = work3;
                questData.SetCurrentFinishOpt2(work2);
            }
            else if (col == 7)
            {
                std::vector<std::string> work = questData.GetFinishEvent();
                work.push_back(buffComma);
                questData.SetFinishEvent(work);
            }

            if (doubleQuoteMode == false)
            {
                col++;
            }
        }
        // 0,1,2,3,4,5,6,7
        // 通常のクエストデータは上記のようなデータであるが
        // 0,1,2,3,4,5,6,
        // 上記のように最後の列に何もない時がある。
        // このときgetline関数が'6'までしか読めない。
        if ((col == 7 || col == 8) && (doubleQuoteMode == false))
        {
            row++;
            col = 0;
            m_vecQuestData.push_back(questData);
            questData = QuestData();
        }
    }

    return true;
}

std::vector<std::string> QuestSystem::GetStartQuest()
{
    std::vector<std::string> result;
    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        if (m_vecQuestData.at(i).GetState() == eQuestState::START)
        {
            result.push_back(m_vecQuestData.at(i).GetId());
            m_vecQuestData.at(i).SetState(eQuestState::STARTED);
        }
    }
    return result;
}

std::vector<std::string> QuestSystem::GetFinishQuest()
{
    std::vector<std::string> result;
    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        if (m_vecQuestData.at(i).GetState() == eQuestState::FINISH)
        {
            result.push_back(m_vecQuestData.at(i).GetId());
            m_vecQuestData.at(i).SetState(eQuestState::FINISHED);
        }
    }
    return result;
}

void QuestSystem::SetTalk(const std::string& npc)
{
    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        for (std::size_t j = 0; j < m_vecQuestData.at(i).GetStartType().size(); ++j)
        {
            if (m_vecQuestData.at(i).GetStartType().at(j) == eStartType::TALK)
            {
                if (m_vecQuestData.at(i).GetStartOption1().at(j) == npc)
                {
                    std::deque<bool> work = m_vecQuestData.at(i).GetStartFlag();
                    work.at(j) = true;
                    m_vecQuestData.at(i).SetStartFlag(work);
                }
            }
        }
    }

    // 話しかけることはクエストの開始条件でもあり、終了条件でもある
    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        // 開始済みのクエストの完了フラグが全部trueならクエスト完了とする
        if (m_vecQuestData.at(i).GetState() == eQuestState::STARTED ||
            m_vecQuestData.at(i).GetState() == eQuestState::START)
        {
            for (std::size_t j = 0; j < m_vecQuestData.at(i).GetFinishType().size(); ++j)
            {
                if (m_vecQuestData.at(i).GetFinishType().at(j) == eFinishType::TALK)
                {
                    if (m_vecQuestData.at(i).GetFinishOption1().at(j) == npc)
                    {
                        std::deque<bool> work = m_vecQuestData.at(i).GetFinishFlag();
                        work.at(j) = true;
                        m_vecQuestData.at(i).SetFinishFlag(work);
                    }
                }
            }
        }
    }
    UpdateQuestStatus();
}

// TODO この関数を呼ぶたびに結果が分かってしまうのはよくない。
void QuestSystem::UpdateQuestStatus()
{
    // クエスト開始チェック
    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        // 全部trueならクエスト開始とする
        bool allTrue = true;
        for (std::size_t j = 0; j < m_vecQuestData.at(i).GetStartType().size(); ++j)
        {
            if (m_vecQuestData.at(i).GetStartFlag().at(j) == false)
            {
                allTrue = false;
                break;
            }
        }
        if (allTrue)
        {
            m_vecQuestData.at(i).SetState(eQuestState::START);
        }
    }

    // クエスト完了チェック
    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        // 開始済みのクエストの完了フラグが全部trueならクエスト完了とする
        if (m_vecQuestData.at(i).GetState() == eQuestState::STARTED ||
            m_vecQuestData.at(i).GetState() == eQuestState::START)
        {
            bool allTrue = true;
            for (std::size_t j = 0; j < m_vecQuestData.at(i).GetFinishType().size(); ++j)
            {
                if (m_vecQuestData.at(i).GetFinishFlag().at(j) == false)
                {
                    allTrue = false;
                    break;
                }
            }
            if (allTrue)
            {
                m_vecQuestData.at(i).SetState(eQuestState::FINISH);
            }
        }
    }
}

void QuestSystem::SetDefeatEnemy(const std::string& enemy)
{
    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        if (m_vecQuestData.at(i).GetState() == eQuestState::STARTED ||
            m_vecQuestData.at(i).GetState() == eQuestState::START)
        {
            for (std::size_t j = 0; j < m_vecQuestData.at(i).GetFinishType().size(); ++j)
            {
                if (m_vecQuestData.at(i).GetFinishType().at(j) == eFinishType::DEFEAT_ENEMY)
                {
                    if (m_vecQuestData.at(i).GetFinishOption1().at(j) == enemy)
                    {
                        std::unordered_map<int, int> work2 = m_vecQuestData.at(i).GetCurrentFinishOpt2();
                        work2[j]--;
                        m_vecQuestData.at(i).SetCurrentFinishOpt2(work2);
                        if (work2[j] <= 0)
                        {
                            std::deque<bool> work = m_vecQuestData.at(i).GetFinishFlag();
                            work.at(j) = true;
                            m_vecQuestData.at(i).SetFinishFlag(work);
                        }
                    }
                }
            }
        }
    }
    UpdateQuestStatus();
}

std::vector<std::string> QuestSystem::GetQuestStartEvent(const std::string& id)
{
    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        if (m_vecQuestData.at(i).GetId() == id)
        {
            return m_vecQuestData.at(i).GetStartEvent();
        }
    }
    return std::vector<std::string>();
}

std::vector<std::string> QuestSystem::GetQuestFinishEvent(const std::string& id)
{
    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        if (m_vecQuestData.at(i).GetId() == id)
        {
            return m_vecQuestData.at(i).GetFinishEvent();
        }
    }
    return std::vector<std::string>();
}

