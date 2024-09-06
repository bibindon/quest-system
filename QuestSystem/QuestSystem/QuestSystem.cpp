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
                questData.m_id = buffComma;
            }
            else if (col == 1)
            {
                if (buffComma == "人と話したら")
                {
                    questData.m_vecStartType.push_back(eStartType::TALK);
                    questData.m_vecStartFlag.push_back(false);
                }
                else if (buffComma == "クエストが完了していたら")
                {
                    questData.m_vecStartType.push_back(eStartType::QUEST_FINISHED);
                    questData.m_vecStartFlag.push_back(false);
                }
                else if (buffComma == "クエストが完了していないなら")
                {
                    questData.m_vecStartType.push_back(eStartType::QUEST_NOT_FINISHED);
                    questData.m_vecStartFlag.push_back(true);
                }
            }
            else if (col == 2)
            {
                questData.m_vecStartOption1.push_back(buffComma);
            }
            else if (col == 3)
            {
                questData.m_vecStartEvent.push_back(buffComma);
            }
            else if (col == 4)
            {
                if (buffComma == "敵を倒したら")
                {
                    questData.m_vecFinishType.push_back(eFinishType::DEFEAT_ENEMY);
                    questData.m_vecFinishFlag.push_back(false);
                }
                else if (buffComma == "人と話したら")
                {
                    questData.m_vecFinishType.push_back(eFinishType::TALK);
                    questData.m_vecFinishFlag.push_back(false);
                }
                else if (buffComma == "自動完了")
                {
                    questData.m_vecFinishType.push_back(eFinishType::AUTO);
                    questData.m_vecFinishFlag.push_back(true);
                }
            }
            else if (col == 5)
            {
                questData.m_vecFinishOption1.push_back(buffComma);
            }
            else if (col == 6)
            {
                questData.m_vecFinishOption2.push_back(buffComma);
            }
            else if (col == 7)
            {
                questData.m_vecFinishEvent.push_back(buffComma);
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
        if (m_vecQuestData.at(i).m_eQuestState == eQuestState::START)
        {
            result.push_back(m_vecQuestData.at(i).m_id);
            m_vecQuestData.at(i).m_eQuestState = eQuestState::STARTED;
        }
    }
    return result;
}

std::vector<std::string> QuestSystem::GetFinishQuest()
{
    std::vector<std::string> result;
    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        if (m_vecQuestData.at(i).m_eQuestState == eQuestState::FINISH)
        {
            result.push_back(m_vecQuestData.at(i).m_id);
            m_vecQuestData.at(i).m_eQuestState = eQuestState::FINISHED;
        }
    }
    return result;
}

void QuestSystem::SetTalk(const std::string& npc)
{
    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        for (std::size_t j = 0; j < m_vecQuestData.at(i).m_vecStartType.size(); ++j)
        {
            if (m_vecQuestData.at(i).m_vecStartType.at(j) == eStartType::TALK)
            {
                if (m_vecQuestData.at(i).m_vecStartOption1.at(j) == npc)
                {
                    m_vecQuestData.at(i).m_vecStartFlag.at(j) = true;
                }
            }
        }
    }

    // 話しかけることはクエストの開始条件でもあり、終了条件でもある
    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        // 開始済みのクエストの完了フラグが全部trueならクエスト完了とする
        if (m_vecQuestData.at(i).m_eQuestState == eQuestState::STARTED || m_vecQuestData.at(i).m_eQuestState == eQuestState::START)
        {
            for (std::size_t j = 0; j < m_vecQuestData.at(i).m_vecFinishType.size(); ++j)
            {
                if (m_vecQuestData.at(i).m_vecFinishType.at(j) == eFinishType::TALK)
                {
                    if (m_vecQuestData.at(i).m_vecFinishOption1.at(j) == npc)
                    {
                        m_vecQuestData.at(i).m_vecFinishFlag.at(j) = true;
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
        for (std::size_t j = 0; j < m_vecQuestData.at(i).m_vecStartType.size(); ++j)
        {
            size_t cnt = std::count(
                m_vecQuestData.at(i).m_vecStartFlag.begin(),
                m_vecQuestData.at(i).m_vecStartFlag.end(),
                false);
            if (cnt == 0)
            {
                m_vecQuestData.at(i).m_eQuestState = eQuestState::START;
            }
        }
    }

    // クエスト完了チェック
    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        // 開始済みのクエストの完了フラグが全部trueならクエスト完了とする
        if (m_vecQuestData.at(i).m_eQuestState == eQuestState::STARTED || m_vecQuestData.at(i).m_eQuestState == eQuestState::START)
        {
            for (std::size_t j = 0; j < m_vecQuestData.at(i).m_vecFinishType.size(); ++j)
            {
                size_t cnt = std::count(
                    m_vecQuestData.at(i).m_vecFinishFlag.begin(),
                    m_vecQuestData.at(i).m_vecFinishFlag.end(),
                    false);
                if (cnt == 0)
                {
                    m_vecQuestData.at(i).m_eQuestState = eQuestState::FINISH;
                }
            }
        }
    }
}

void QuestSystem::SetDefeatEnemy(const std::string& enemy)
{
    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        if (m_vecQuestData.at(i).m_eQuestState == eQuestState::STARTED || m_vecQuestData.at(i).m_eQuestState == eQuestState::START)
        {
            for (std::size_t j = 0; j < m_vecQuestData.at(i).m_vecFinishType.size(); ++j)
            {
                if (m_vecQuestData.at(i).m_vecFinishType.at(j) == eFinishType::DEFEAT_ENEMY)
                {
                    if (m_vecQuestData.at(i).m_vecFinishOption1.at(j) == enemy)
                    {
                        m_vecQuestData.at(i).m_vecFinishFlag.at(j) = true;
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
        if (m_vecQuestData.at(i).m_id == id)
        {
            return m_vecQuestData.at(i).m_vecStartEvent;
        }
    }
    return std::vector<std::string>();
}

std::vector<std::string> QuestSystem::GetQuestFinishEvent(const std::string& id)
{
    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        if (m_vecQuestData.at(i).m_id == id)
        {
            return m_vecQuestData.at(i).m_vecFinishEvent;
        }
    }
    return std::vector<std::string>();
}

