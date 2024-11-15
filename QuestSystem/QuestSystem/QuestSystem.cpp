#include "QuestSystem.h"
#include <windows.h>
#include <fstream>
#include <sstream>
#include "Shlwapi.h"

#pragma comment( lib, "Shlwapi.lib" ) 

using namespace NSQuestSystem;

static std::vector<std::string> split(const std::string& s, char delim)
{
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string item;

    while (getline(ss, item, delim))
    {
        result.push_back(item);
    }

    return result;
}

QuestSystem::QuestSystem()
{
}

static void ltrim(std::string& s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
        [](unsigned char ch)
        {
            return !std::isspace(ch);
        }
    ));
}

static void rtrim(std::string& s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(),
        [](unsigned char ch)
        {
            return !std::isspace(ch);
        }
    ).base(), s.end());
}

static void trim(std::string& s)
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
                if (buffComma.empty())
                {
                    break;
                }
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
                else if (buffComma == "位置")
                {
                    work.push_back(eStartType::POS);
                    work2.push_back(false);
                }
                else if (buffComma == "調べたら")
                {
                    work.push_back(eStartType::EXAMINE);
                    work2.push_back(false);
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
                else if (buffComma == "位置")
                {
                    work.push_back(eFinishType::POS);
                    work2.push_back(false);
                }
                else if (buffComma == "クエストが完了していたら")
                {
                    work.push_back(eFinishType::QUEST_FINISHED);
                    work2.push_back(false);
                }
                else if (buffComma == "調べたら")
                {
                    work.push_back(eFinishType::EXAMINE);
                    work2.push_back(false);
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
        if ((col >= 7) && (doubleQuoteMode == false))
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

void NSQuestSystem::QuestSystem::SetPos(const float x, const float y, const float z)
{
    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        if (m_vecQuestData.at(i).GetState() == eQuestState::NOT_START)
        {
            for (std::size_t j = 0; j < m_vecQuestData.at(i).GetStartType().size(); ++j)
            {
                if (m_vecQuestData.at(i).GetStartType().at(j) == eStartType::POS)
                {
                    // 0.0:1.0:2.0:3.0だったら座標(0.0, 1.0, 2.0)で半径が3.0、の意味
                    std::string xyzr = m_vecQuestData.at(i).GetStartOption1().at(j);
                    std::vector<std::string> vs = split(xyzr, ':');
                    float startX = std::stof(vs.at(0));
                    float startY = std::stof(vs.at(1));
                    float startZ = std::stof(vs.at(2));
                    float startR = std::stof(vs.at(3));

                    float dx = startX - x;
                    float dy = startY - y;
                    float dz = startZ - z;

                    float r = std::sqrt(dx * dx + dy * dy + dz * dz);

                    if (r <= startR)
                    {
                        std::deque<bool> work = m_vecQuestData.at(i).GetStartFlag();
                        work.at(j) = true;
                        m_vecQuestData.at(i).SetStartFlag(work);
                    }
                }
            }
        }
    }

    // 座標に到達したことはクエストの開始条件でもあり、終了条件でもある
    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        // 開始済みのクエストの完了フラグが全部trueならクエスト完了とする
        if (m_vecQuestData.at(i).GetState() == eQuestState::STARTED ||
            m_vecQuestData.at(i).GetState() == eQuestState::START)
        {
            for (std::size_t j = 0; j < m_vecQuestData.at(i).GetFinishType().size(); ++j)
            {
                if (m_vecQuestData.at(i).GetFinishType().at(j) == eFinishType::POS)
                {
                    // 0.0:1.0:2.0:3.0だったら座標(0.0, 1.0, 2.0)で半径が3.0、の意味
                    std::string xyzr = m_vecQuestData.at(i).GetFinishOption1().at(j);
                    std::vector<std::string> vs = split(xyzr, ':');
                    float startX = std::stof(vs.at(0));
                    float startY = std::stof(vs.at(1));
                    float startZ = std::stof(vs.at(2));
                    float startR = std::stof(vs.at(3));

                    float dx = startX - x;
                    float dy = startY - y;
                    float dz = startZ - z;

                    float r = std::sqrt(dx * dx + dy * dy + dz * dz);

                    if (r <= startR)
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
        if (allTrue && m_vecQuestData.at(i).GetState() == eQuestState::NOT_START)
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

    // クエストの開始条件「クエストが完了していたら」の処理
    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        if (m_vecQuestData.at(i).GetState() == eQuestState::NOT_START)
        {
            std::vector<eStartType> startType = m_vecQuestData.at(i).GetStartType();
            for (std::size_t j = 0; j < startType.size(); ++j)
            {
                if (startType.at(j) == eStartType::QUEST_FINISHED)
                {
                    std::vector<std::string> vs = m_vecQuestData.at(i).GetStartOption1();
                    std::string questId = vs.at(j);
                    for (std::size_t k = 0; k < m_vecQuestData.size(); ++k)
                    {
                        if (m_vecQuestData.at(k).GetId() == questId)
                        {
                            if (m_vecQuestData.at(k).GetState() == eQuestState::FINISH)
                            {
                                std::deque<bool> work = m_vecQuestData.at(i).GetStartFlag();
                                work.at(j) = true;
                                m_vecQuestData.at(i).SetStartFlag(work);
                            }
                        }
                    }
                }
            }
        }
    }

    // クエストの完了条件「クエストが完了していたら」の処理
    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        if (m_vecQuestData.at(i).GetState() == eQuestState::START ||
            m_vecQuestData.at(i).GetState() == eQuestState::STARTED)
        {
            std::vector<eFinishType> finishType = m_vecQuestData.at(i).GetFinishType();
            for (std::size_t j = 0; j < finishType.size(); ++j)
            {
                if (finishType.at(j) == eFinishType::QUEST_FINISHED)
                {
                    std::vector<std::string> vs = m_vecQuestData.at(i).GetFinishOption1();
                    std::string questId = vs.at(j);
                    for (std::size_t k = 0; k < m_vecQuestData.size(); ++k)
                    {
                        if (m_vecQuestData.at(k).GetId() == questId)
                        {
                            if (m_vecQuestData.at(k).GetState() == eQuestState::FINISH)
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
    }

    // クエストの開始条件「クエストが完了していたら」、
    // クエストの完了条件「クエストが完了していたら」のチェックを行ったうえで
    // 改めて、クエスト開始・完了チェック
 
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
        if (allTrue && m_vecQuestData.at(i).GetState() == eQuestState::NOT_START)
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

void NSQuestSystem::QuestSystem::SetExamine(const float x, const float y, const float z)
{
    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        if (m_vecQuestData.at(i).GetState() == eQuestState::NOT_START)
        {
            for (std::size_t j = 0; j < m_vecQuestData.at(i).GetStartType().size(); ++j)
            {
                if (m_vecQuestData.at(i).GetStartType().at(j) == eStartType::EXAMINE)
                {
                    // 0.0:1.0:2.0:3.0だったら座標(0.0, 1.0, 2.0)で半径が3.0、の意味
                    std::string xyzr = m_vecQuestData.at(i).GetStartOption1().at(j);
                    std::vector<std::string> vs = split(xyzr, ':');
                    float startX = std::stof(vs.at(0));
                    float startY = std::stof(vs.at(1));
                    float startZ = std::stof(vs.at(2));
                    float startR = std::stof(vs.at(3));

                    float dx = startX - x;
                    float dy = startY - y;
                    float dz = startZ - z;

                    float r = std::sqrt(dx * dx + dy * dy + dz * dz);

                    if (r <= startR)
                    {
                        std::deque<bool> work = m_vecQuestData.at(i).GetStartFlag();
                        work.at(j) = true;
                        m_vecQuestData.at(i).SetStartFlag(work);
                    }
                }
            }
        }
    }

    // 座標に到達したことはクエストの開始条件でもあり、終了条件でもある
    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        // 開始済みのクエストの完了フラグが全部trueならクエスト完了とする
        if (m_vecQuestData.at(i).GetState() == eQuestState::STARTED ||
            m_vecQuestData.at(i).GetState() == eQuestState::START)
        {
            for (std::size_t j = 0; j < m_vecQuestData.at(i).GetFinishType().size(); ++j)
            {
                if (m_vecQuestData.at(i).GetFinishType().at(j) == eFinishType::EXAMINE)
                {
                    // 0.0:1.0:2.0:3.0だったら座標(0.0, 1.0, 2.0)で半径が3.0、の意味
                    std::string xyzr = m_vecQuestData.at(i).GetFinishOption1().at(j);
                    std::vector<std::string> vs = split(xyzr, ':');
                    float startX = std::stof(vs.at(0));
                    float startY = std::stof(vs.at(1));
                    float startZ = std::stof(vs.at(2));
                    float startR = std::stof(vs.at(3));

                    float dx = startX - x;
                    float dy = startY - y;
                    float dz = startZ - z;

                    float r = std::sqrt(dx * dx + dy * dy + dz * dz);

                    if (r <= startR)
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

std::string NSQuestSystem::QuestSystem::GetQuestIdStartByExamine(const float x,
                                                                 const float y,
                                                                 const float z)
{
    std::string result;
    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        if (m_vecQuestData.at(i).GetState() == eQuestState::NOT_START)
        {
            for (std::size_t j = 0; j < m_vecQuestData.at(i).GetStartType().size(); ++j)
            {
                if (m_vecQuestData.at(i).GetStartType().at(j) == eStartType::EXAMINE)
                {
                    // 0.0:1.0:2.0:3.0だったら座標(0.0, 1.0, 2.0)で半径が3.0、の意味
                    std::string xyzr = m_vecQuestData.at(i).GetStartOption1().at(j);
                    std::vector<std::string> vs = split(xyzr, ':');
                    float startX = std::stof(vs.at(0));
                    float startY = std::stof(vs.at(1));
                    float startZ = std::stof(vs.at(2));
                    float startR = std::stof(vs.at(3));

                    float dx = startX - x;
                    float dy = startY - y;
                    float dz = startZ - z;

                    float r = std::sqrt(dx * dx + dy * dy + dz * dz);

                    if (r <= startR)
                    {
                        result = m_vecQuestData.at(i).GetId();
                        break;
                    }
                }
            }
        }
    }
    return result;
}

std::string NSQuestSystem::QuestSystem::GetQuestIdFinishByExamine(const float x,
                                                                  const float y,
                                                                  const float z)
{
    std::string result;
    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        // 開始済みのクエストの完了フラグが全部trueならクエスト完了とする
        if (m_vecQuestData.at(i).GetState() == eQuestState::STARTED ||
            m_vecQuestData.at(i).GetState() == eQuestState::START)
        {
            for (std::size_t j = 0; j < m_vecQuestData.at(i).GetFinishType().size(); ++j)
            {
                if (m_vecQuestData.at(i).GetFinishType().at(j) == eFinishType::EXAMINE)
                {
                    // 0.0:1.0:2.0:3.0だったら座標(0.0, 1.0, 2.0)で半径が3.0、の意味
                    std::string xyzr = m_vecQuestData.at(i).GetFinishOption1().at(j);
                    std::vector<std::string> vs = split(xyzr, ':');
                    float startX = std::stof(vs.at(0));
                    float startY = std::stof(vs.at(1));
                    float startZ = std::stof(vs.at(2));
                    float startR = std::stof(vs.at(3));

                    float dx = startX - x;
                    float dy = startY - y;
                    float dz = startZ - z;

                    float r = std::sqrt(dx * dx + dy * dy + dz * dz);

                    if (r <= startR)
                    {
                        result = m_vecQuestData.at(i).GetId();
                        break;
                    }
                }
            }
        }
    }
    return result;
}

