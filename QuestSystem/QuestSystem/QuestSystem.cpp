#include "QuestSystem.h"
#include <windows.h>
#include <fstream>
#include <sstream>
#include "Shlwapi.h"
#include "CaesarCipher.h"
#include "HeaderOnlyCsv.hpp"
#include <tchar.h>
#include <cwctype>

#pragma comment( lib, "Shlwapi.lib" ) 

using namespace NSQuestSystem;

std::wstring Utf8ToWstring(const std::string& utf8)
{
    if (utf8.empty())
    {
        return std::wstring();
    }

    int len = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0);
    if (len == 0)
    {
        throw std::runtime_error("UTF-8 to UTF-16 conversion failed.");
    }

    std::wstring result(len - 1, 0);
    MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, &result[0], len);

    return result;
}

static std::vector<std::wstring> split(const std::wstring& s, wchar_t delim)
{
    std::vector<std::wstring> result;
    std::wstringstream ss(s);
    std::wstring item;

    while (getline(ss, item, delim))
    {
        result.push_back(item);
    }

    return result;
}

QuestSystem::QuestSystem()
{
}

static void ltrim(std::wstring& s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
        [](wchar_t ch)
        {
            return !std::iswspace(ch);
        }
    ));
}

static void rtrim(std::wstring& s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(),
        [](wchar_t ch)
        {
            return !std::iswspace(ch);
        }
    ).base(), s.end());
}

static void trim(std::wstring& s)
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
bool QuestSystem::Init(const std::wstring& csvFilePath,
                       const std::wstring& savefile,
                       const bool encrypt)
{
    int result = PathFileExists(csvFilePath.c_str());
    if (result == 0)
    {
        throw std::exception();
    }

    std::stringstream ss;
    std::string content;
    std::wstring wcontent;

    if (encrypt == false)
    {
        std::ifstream ifs(csvFilePath);
        ss << ifs.rdbuf();
        ifs.close();
        
        content = ss.str();

        // BOM削除（0xEF 0xBB 0xBF）
        if (content.size() >= 3 &&
            static_cast<unsigned char>(content[0]) == 0xEF &&
            static_cast<unsigned char>(content[1]) == 0xBB &&
            static_cast<unsigned char>(content[2]) == 0xBF)
        {
            content = content.substr(3);
        }

        wcontent = Utf8ToWstring(content);
    }
    else
    {
        wcontent = CaesarCipher::DecryptFromFile(csvFilePath);
    }

    std::wstring buff;
    std::wstring buffComma;
    QuestData questData;
    int row = 0;
    int col = 0;
    bool doubleQuote = false;
    bool doubleQuoteMode = false;

    std::wstringstream wss(wcontent);

    while (std::getline(wss, buff))
    {
        // 先頭行は無視
        if (row == 0)
        {
            row = 1;
            continue;
        }
        std::wistringstream iss(buff);

        while (std::getline(iss, buffComma, L','))
        {
            trim(buffComma);
            if (buffComma.find(L'"') != std::wstring::npos)
            {
                doubleQuote = true;

                if (buffComma.at(0) == L'"')
                {
                    doubleQuoteMode = true;
                }
                else
                {
                    doubleQuoteMode = false;
                }
                buffComma.erase(std::remove(buffComma.begin(), buffComma.end(), L'"'), buffComma.end());
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
                if (buffComma == _T("人と話したら"))
                {
                    work.push_back(eStartType::TALK);
                    work2.push_back(false);
                }
                else if (buffComma == _T("クエストが完了していたら"))
                {
                    work.push_back(eStartType::QUEST_FINISHED);
                    work2.push_back(false);
                }
                else if (buffComma == _T("クエストが完了していないなら"))
                {
                    work.push_back(eStartType::QUEST_NOT_FINISHED);
                    work2.push_back(true);
                }
                else if (buffComma == _T("位置"))
                {
                    work.push_back(eStartType::POS);
                    work2.push_back(false);
                }
                else if (buffComma == _T("調べたら"))
                {
                    work.push_back(eStartType::EXAMINE);
                    work2.push_back(false);
                }
                else if (buffComma == _T("一つでもクエストが完了していたら"))
                {
                    work.push_back(eStartType::QUEST_FINISH_OR);
                    work2.push_back(false);
                }
                else if (buffComma == _T("インベントリにXがY個あったら"))
                {
                    work.push_back(eStartType::INVENTORY);
                    work2.push_back(false);
                }
                else if (buffComma == _T("倉庫AにXがY個あったら"))
                {
                    work.push_back(eStartType::STOREHOUSE);
                    work2.push_back(false);
                }
                else if (buffComma == _T("インベントリに強化値XのYがZ個あったら"))
                {
                    work.push_back(eStartType::INVENTORY_LEVEL);
                    work2.push_back(false);
                }
                else if (buffComma == _T("倉庫Aに強化値XのYがZ個あったら"))
                {
                    work.push_back(eStartType::STOREHOUSE_LEVEL);
                    work2.push_back(false);
                }
                else if (buffComma == _T("体の体力がX以下だったら"))
                {
                    work.push_back(eStartType::BODY_STAMINA_LESS);
                    work2.push_back(false);
                }
                else if (buffComma == _T("脳の体力がX以下だったら"))
                {
                    work.push_back(eStartType::BRAIN_STAMINA_LESS);
                    work2.push_back(false);
                }
                else if (buffComma == _T("位置が範囲外"))
                {
                    work.push_back(eStartType::POS_OUT);
                    work2.push_back(false);
                }
                else
                {
                    int size = WideCharToMultiByte(CP_UTF8, 0, buffComma.c_str(), -1, nullptr, 0, nullptr, nullptr);
                    std::string result(size - 1, 0);
                    WideCharToMultiByte(CP_UTF8, 0, buffComma.c_str(), -1, &result[0], size, nullptr, nullptr);

                    throw std::exception(result.c_str());
                }
                questData.SetStartType(work);
                questData.SetStartFlag(work2);
            }
            else if (col == 2)
            {
                std::vector<std::wstring> work = questData.GetStartOption1();
                work.push_back(buffComma);
                questData.SetStartOption1(work);
            }
            else if (col == 3)
            {
                std::vector<std::wstring> work = questData.GetStartEvent();
                work.push_back(buffComma);
                questData.SetStartEvent(work);
            }
            else if (col == 4)
            {
                std::vector<eFinishType> work = questData.GetFinishType();
                std::deque<bool> work2 = questData.GetFinishFlag();
                if (buffComma == _T("敵を倒したら"))
                {
                    work.push_back(eFinishType::DEFEAT_ENEMY);
                    work2.push_back(false);
                }
                else if (buffComma == _T("人と話したら"))
                {
                    work.push_back(eFinishType::TALK);
                    work2.push_back(false);
                }
                else if (buffComma == _T("自動完了"))
                {
                    work.push_back(eFinishType::AUTO);
                    work2.push_back(true);
                }
                else if (buffComma == _T("位置"))
                {
                    work.push_back(eFinishType::POS);
                    work2.push_back(false);
                }
                else if (buffComma == _T("クエストが完了していたら"))
                {
                    work.push_back(eFinishType::QUEST_FINISHED);
                    work2.push_back(false);
                }
                else if (buffComma == _T("調べたら"))
                {
                    work.push_back(eFinishType::EXAMINE);
                    work2.push_back(false);
                }
                else if (buffComma == _T("インベントリにXがY個あったら"))
                {
                    work.push_back(eFinishType::INVENTORY);
                    work2.push_back(false);
                }
                else if (buffComma == _T("倉庫AにXがY個あったら"))
                {
                    work.push_back(eFinishType::STOREHOUSE);
                    work2.push_back(false);
                }
                else if (buffComma == _T("インベントリに強化値XのYがZ個あったら"))
                {
                    work.push_back(eFinishType::INVENTORY_LEVEL);
                    work2.push_back(false);
                }
                else if (buffComma == _T("倉庫Aに強化値XのYがZ個あったら"))
                {
                    work.push_back(eFinishType::STOREHOUSE_LEVEL);
                    work2.push_back(false);
                }
                else if (buffComma == _T("体の体力がX以下だったら"))
                {
                    work.push_back(eFinishType::BODY_STAMINA_LESS);
                    work2.push_back(false);
                }
                else if (buffComma == _T("脳の体力がX以下だったら"))
                {
                    work.push_back(eFinishType::BRAIN_STAMINA_LESS);
                    work2.push_back(false);
                }
                else if (buffComma == _T("位置が範囲外"))
                {
                    work.push_back(eFinishType::POS_OUT);
                    work2.push_back(false);
                }
                else if (buffComma == _T("時間が経過したら"))
                {
                    work.push_back(eFinishType::TIME_PAST);
                    work2.push_back(false);
                }
                else if (buffComma == _T("夜だったら"))
                {
                    work.push_back(eFinishType::AT_NIGHT);
                    work2.push_back(false);
                }
                else if (buffComma == _T("昼だったら"))
                {
                    work.push_back(eFinishType::AT_DAYTIME);
                    work2.push_back(false);
                }
                else if (buffComma == _T("NPCのXが生存"))
                {
                    work.push_back(eFinishType::NPC_ALIVE);
                    work2.push_back(false);
                }
                else if (buffComma == _T("イカダの個数"))
                {
                    work.push_back(eFinishType::RAFT_NUM);
                    work2.push_back(false);
                }
                else
                {
                    int size = WideCharToMultiByte(CP_UTF8, 0, buffComma.c_str(), -1, nullptr, 0, nullptr, nullptr);
                    std::string result(size - 1, 0);
                    WideCharToMultiByte(CP_UTF8, 0, buffComma.c_str(), -1, &result[0], size, nullptr, nullptr);

                    throw std::exception(result.c_str());
                }
                questData.SetFinishType(work);
                questData.SetFinishFlag(work2);
            }
            else if (col == 5)
            {
                std::vector<std::wstring> work = questData.GetFinishOption1();
                work.push_back(buffComma);
                questData.SetFinishOption1(work);
            }
            else if (col == 6)
            {
                std::vector<std::wstring> work = questData.GetFinishOption2();
                work.push_back(buffComma);
                questData.SetFinishOption2(work);

                std::unordered_map<int, int> work2 = questData.GetCurrentFinishOpt2();
                int work3 = 0;
                std::wstringstream(buffComma) >> work3;
                work2[(int)work.size() - 1] = work3;
                questData.SetCurrentFinishOpt2(work2);
            }
            else if (col == 7)
            {
                std::vector<std::wstring> work = questData.GetFinishEvent();
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

    if (savefile.empty())
    {
        return true;
    }

    // セーブデータを読む
    {
        std::vector<std::vector<std::wstring>> vvs;
        if (encrypt == false)
        {
            vvs = csv::Read(savefile);
        }
        else
        {
            std::wstring work = CaesarCipher::DecryptFromFile(savefile);
            vvs = csv::ReadFromString(work);
        }

        for (size_t i = 1; i < vvs.size(); ++i)
        {
            auto it = std::find_if(m_vecQuestData.begin(), m_vecQuestData.end(),
                                   [&](const QuestData& x)
                                   {
                                       return x.GetId() == vvs.at(i).at(0);
                                   });

            if (it == m_vecQuestData.end())
            {
                throw std::exception();
            }

            if (vvs.at(i).at(1) == _T("NOT_START"))
            {
                it->SetState(eQuestState::NOT_START);
            }
            else if (vvs.at(i).at(1) == _T("START"))
            {
                it->SetState(eQuestState::START);
            }
            else if (vvs.at(i).at(1) == _T("STARTED"))
            {
                it->SetState(eQuestState::STARTED);
            }
            else if (vvs.at(i).at(1) == _T("FINISH"))
            {
                it->SetState(eQuestState::FINISH);
            }
            else if (vvs.at(i).at(1) == _T("FINISHED"))
            {
                it->SetState(eQuestState::FINISHED);
            }

            {
                std::wstring work = vvs.at(i).at(2);
                auto vs = split(work, ':');

                int startYear = std::stoi(vs.at(0));
                int startMonth = std::stoi(vs.at(1));
                int startDay = std::stoi(vs.at(2));
                int startHour = std::stoi(vs.at(3));
                int startMinute = std::stoi(vs.at(4));
                int startSecond = std::stoi(vs.at(5));

                it->SetStartDateTime(startYear, startMonth, startDay, startHour, startMinute, startSecond);
            }
        }
    }

    return true;
}

void NSQuestSystem::QuestSystem::Save(const std::wstring& filename, const bool encrypt)
{
    std::vector<std::vector<std::wstring>> vvs;
    std::vector<std::wstring> vs;
    vs.push_back(_T("クエストID"));
    vs.push_back(_T("状況"));
    vs.push_back(_T("クエスト開始時刻"));
    vvs.push_back(vs);
    vs.clear();

    for (size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        std::wstring work;
        work = m_vecQuestData.at(i).GetId();
        vs.push_back(work);

        if (m_vecQuestData.at(i).GetState() == eQuestState::NOT_START)
        {
            vs.push_back(_T("NOT_START"));
        }
        else if (m_vecQuestData.at(i).GetState() == eQuestState::START)
        {
            vs.push_back(_T("START"));
        }
        else if (m_vecQuestData.at(i).GetState() == eQuestState::STARTED)
        {
            vs.push_back(_T("STARTED"));
        }
        else if (m_vecQuestData.at(i).GetState() == eQuestState::FINISH)
        {
            vs.push_back(_T("FINISH"));
        }
        else if (m_vecQuestData.at(i).GetState() == eQuestState::FINISHED)
        {
            vs.push_back(_T("FINISHED"));
        }

        {
            int startYear = 0;
            int startMonth = 0;
            int startDay = 0;
            int startHour = 0;
            int startMinute = 0;
            int startSecond = 0;

            m_vecQuestData.at(i).GetStartDateTime(&startYear, &startMonth, &startDay, &startHour, &startMinute, &startSecond);

            std::wstring year = std::to_wstring(startYear);
            std::wstring month = std::to_wstring(startMonth);
            std::wstring day = std::to_wstring(startDay);
            std::wstring hour = std::to_wstring(startHour);
            std::wstring minute = std::to_wstring(startMinute);
            std::wstring second = std::to_wstring(startSecond);

            std::wstring work;
            work = year + _T(":");
            work += month + _T(":");
            work += day + _T(":");
            work += hour + _T(":");
            work += minute + _T(":");
            work += second;

            vs.push_back(work);
        }

        vvs.push_back(vs);
        vs.clear();
    }

    if (encrypt == false)
    {
        csv::Write(filename, vvs);
    }
    else
    {
        std::wstringstream ss;
        for (std::size_t i = 0; i < vvs.size(); ++i)
        {
            for (std::size_t j = 0; j < vvs.at(i).size(); ++j)
            {
                ss << vvs.at(i).at(j);
                if (j != vvs.at(i).size() - 1)
                {
                    ss << ",";
                }
            }
            ss << "\n";
        }
        CaesarCipher::EncryptToFile(ss.str(), filename);
    }
}

std::vector<std::wstring> QuestSystem::GetStartQuest()
{
    std::vector<std::wstring> result;
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

std::vector<std::wstring> NSQuestSystem::QuestSystem::GetStartedQuest()
{
    std::vector<std::wstring> result;
    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        if (m_vecQuestData.at(i).GetState() == eQuestState::STARTED)
        {
            result.push_back(m_vecQuestData.at(i).GetId());
        }
    }

    return result;
}

std::vector<std::wstring> QuestSystem::GetFinishQuest()
{
    std::vector<std::wstring> result;
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

bool NSQuestSystem::QuestSystem::GetQuestFinished(const std::wstring& id)
{
    bool bFinished = false;

    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        if (m_vecQuestData.at(i).GetId() == id)
        {
            if (m_vecQuestData.at(i).GetState() == eQuestState::FINISHED)
            {
                bFinished = true;
            }
            else
            {
                bFinished = false;
            }
            break;
        }
    }

    return bFinished;
}

void QuestSystem::SetTalk(const std::wstring& npc)
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

void NSQuestSystem::QuestSystem::SetPos(const float x, const float y, const float z,
                                        const bool update)
{
    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        if (m_vecQuestData.at(i).GetState() == eQuestState::NOT_START)
        {
            for (std::size_t j = 0; j < m_vecQuestData.at(i).GetStartType().size(); ++j)
            {
                if (m_vecQuestData.at(i).GetStartType().at(j) == eStartType::POS ||
                    m_vecQuestData.at(i).GetStartType().at(j) == eStartType::POS_OUT)
                {
                    // 0.0:1.0:2.0:3.0だったら座標(0.0, 1.0, 2.0)で半径が3.0、の意味
                    std::wstring xyzr = m_vecQuestData.at(i).GetStartOption1().at(j);
                    std::vector<std::wstring> vs = split(xyzr, ':');
                    float startX = std::stof(vs.at(0));
                    float startY = std::stof(vs.at(1));
                    float startZ = std::stof(vs.at(2));
                    float startR = std::stof(vs.at(3));

                    float dx = startX - x;
                    float dy = startY - y;
                    float dz = startZ - z;

                    // 円ではなく立方体で考える
                    bool bHit = false;
                    if (dx <= startR && dz <= startR && dy <= startR)
                    {
                        bHit = true;
                    }

                    if (m_vecQuestData.at(i).GetStartType().at(j) == eStartType::POS)
                    {
                        if (bHit)
                        {
                            std::deque<bool> work = m_vecQuestData.at(i).GetStartFlag();
                            work.at(j) = true;
                            m_vecQuestData.at(i).SetStartFlag(work);
                        }
                        else
                        {
                            std::deque<bool> work = m_vecQuestData.at(i).GetStartFlag();
                            work.at(j) = false;
                            m_vecQuestData.at(i).SetStartFlag(work);
                        }
                    }
                    else if (m_vecQuestData.at(i).GetStartType().at(j) == eStartType::POS_OUT)
                    {
                        if (!bHit)
                        {
                            std::deque<bool> work = m_vecQuestData.at(i).GetStartFlag();
                            work.at(j) = true;
                            m_vecQuestData.at(i).SetStartFlag(work);
                        }
                        else
                        {
                            std::deque<bool> work = m_vecQuestData.at(i).GetStartFlag();
                            work.at(j) = false;
                            m_vecQuestData.at(i).SetStartFlag(work);
                        }
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
                if (m_vecQuestData.at(i).GetFinishType().at(j) == eFinishType::POS ||
                    m_vecQuestData.at(i).GetFinishType().at(j) == eFinishType::POS_OUT)
                {
                    // 0.0:1.0:2.0:3.0だったら座標(0.0, 1.0, 2.0)で半径が3.0、の意味
                    std::wstring xyzr = m_vecQuestData.at(i).GetFinishOption1().at(j);
                    std::vector<std::wstring> vs = split(xyzr, ':');
                    float startX = std::stof(vs.at(0));
                    float startY = std::stof(vs.at(1));
                    float startZ = std::stof(vs.at(2));
                    float startR = std::stof(vs.at(3));

                    float dx = startX - x;
                    float dy = startY - y;
                    float dz = startZ - z;

                    // 円ではなく立方体で考える
                    bool bHit = false;
                    if (dx <= startR && dz <= startR && dy <= startR)
                    {
                        bHit = true;
                    }

                    if (m_vecQuestData.at(i).GetFinishType().at(j) == eFinishType::POS)
                    {
                        if (bHit)
                        {
                            std::deque<bool> work = m_vecQuestData.at(i).GetFinishFlag();
                            work.at(j) = true;
                            m_vecQuestData.at(i).SetFinishFlag(work);
                        }
                        else
                        {
                            std::deque<bool> work = m_vecQuestData.at(i).GetFinishFlag();
                            work.at(j) = false;
                            m_vecQuestData.at(i).SetFinishFlag(work);
                        }
                    }
                    else if (m_vecQuestData.at(i).GetFinishType().at(j) == eFinishType::POS_OUT)
                    {
                        if (!bHit)
                        {
                            std::deque<bool> work = m_vecQuestData.at(i).GetFinishFlag();
                            work.at(j) = true;
                            m_vecQuestData.at(i).SetFinishFlag(work);
                        }
                        else
                        {
                            std::deque<bool> work = m_vecQuestData.at(i).GetFinishFlag();
                            work.at(j) = false;
                            m_vecQuestData.at(i).SetFinishFlag(work);
                        }
                    }
                }
            }
        }
    }

    if (update)
    {
        UpdateQuestStatus();
    }
}

// 重いので頻繁に呼んではいけない。
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
            m_vecQuestData.at(i).SetStartDateTime(m_currentYear, m_currentMonth, m_currentDay,
                                                  m_currentHour, m_currentMinute, m_currentSecond);
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
                    std::vector<std::wstring> vs = m_vecQuestData.at(i).GetStartOption1();
                    std::wstring questId = vs.at(j);
                    for (std::size_t k = 0; k < m_vecQuestData.size(); ++k)
                    {
                        if (m_vecQuestData.at(k).GetId() == questId)
                        {
                            if (m_vecQuestData.at(k).GetState() == eQuestState::FINISH ||
                                m_vecQuestData.at(k).GetState() == eQuestState::FINISHED)
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

    // クエストの開始条件「クエストが完了していないなら」の処理
    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        if (m_vecQuestData.at(i).GetState() == eQuestState::NOT_START)
        {
            std::vector<eStartType> startType = m_vecQuestData.at(i).GetStartType();
            for (std::size_t j = 0; j < startType.size(); ++j)
            {
                if (startType.at(j) == eStartType::QUEST_NOT_FINISHED)
                {
                    std::vector<std::wstring> vs = m_vecQuestData.at(i).GetStartOption1();
                    std::wstring questId = vs.at(j);
                    for (std::size_t k = 0; k < m_vecQuestData.size(); ++k)
                    {
                        if (m_vecQuestData.at(k).GetId() == questId)
                        {
                            if (m_vecQuestData.at(k).GetState() == eQuestState::NOT_START)
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

    // クエストの開始条件「一つでもクエストが完了していたら」の処理
    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        if (m_vecQuestData.at(i).GetState() == eQuestState::NOT_START)
        {
            std::vector<eStartType> startType = m_vecQuestData.at(i).GetStartType();
            for (std::size_t j = 0; j < startType.size(); ++j)
            {
                if (startType.at(j) == eStartType::QUEST_FINISH_OR)
                {
                    std::vector<std::wstring> vs = m_vecQuestData.at(i).GetStartOption1();
                    std::vector<std::wstring> questIds = split(vs.at(j), ':');
                    for (size_t l = 0; l < questIds.size(); ++l)
                    {
                        for (std::size_t k = 0; k < m_vecQuestData.size(); ++k)
                        {
                            if (m_vecQuestData.at(k).GetId() == questIds.at(l))
                            {
                                if (m_vecQuestData.at(k).GetState() == eQuestState::FINISH ||
                                    m_vecQuestData.at(k).GetState() == eQuestState::FINISHED)
                                {
                                    std::deque<bool> work = m_vecQuestData.at(i).GetStartFlag();
                                    work.at(j) = true;
                                    m_vecQuestData.at(i).SetStartFlag(work);
                                    break;
                                }
                            }
                        }

                        if (m_vecQuestData.at(i).GetStartFlag().at(j))
                        {
                            break;
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
                    std::vector<std::wstring> vs = m_vecQuestData.at(i).GetFinishOption1();
                    std::wstring questId = vs.at(j);
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
    // クエストの開始条件「クエストが完了していないなら」、
    // クエストの開始条件「一つでもクエストが完了していたら」、
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
            m_vecQuestData.at(i).SetStartDateTime(m_currentYear, m_currentMonth, m_currentDay,
                                                  m_currentHour, m_currentMinute, m_currentSecond);
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

void QuestSystem::SetDefeatEnemy(const std::wstring& enemy)
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
                        work2[(int)j]--;
                        m_vecQuestData.at(i).SetCurrentFinishOpt2(work2);
                        if (work2[(int)j] <= 0)
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

std::vector<std::wstring> QuestSystem::GetQuestStartEvent(const std::wstring& id)
{
    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        if (m_vecQuestData.at(i).GetId() == id)
        {
            return m_vecQuestData.at(i).GetStartEvent();
        }
    }
    return std::vector<std::wstring>();
}

std::vector<std::wstring> QuestSystem::GetQuestFinishEvent(const std::wstring& id)
{
    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        if (m_vecQuestData.at(i).GetId() == id)
        {
            return m_vecQuestData.at(i).GetFinishEvent();
        }
    }
    return std::vector<std::wstring>();
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
                    std::wstring xyzr = m_vecQuestData.at(i).GetStartOption1().at(j);
                    std::vector<std::wstring> vs = split(xyzr, ':');
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
                    std::wstring xyzr = m_vecQuestData.at(i).GetFinishOption1().at(j);
                    std::vector<std::wstring> vs = split(xyzr, ':');
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

void NSQuestSystem::QuestSystem::SetInventoryContent(const std::vector<ItemInfo>& list, const bool update)
{
    m_inventory = list;

    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        if (m_vecQuestData.at(i).GetState() == eQuestState::NOT_START)
        {
            for (std::size_t j = 0; j < m_vecQuestData.at(i).GetStartType().size(); ++j)
            {
                if (m_vecQuestData.at(i).GetStartType().at(j) == eStartType::INVENTORY)
                {
                    // トマト:5だったらトマトが5個以上あったらの意味
                    std::wstring opt = m_vecQuestData.at(i).GetStartOption1().at(j);
                    std::vector<std::wstring> vs = split(opt, ':');
                    std::wstring itemName = vs.at(0);
                    int num = std::stoi(vs.at(1));

                    int work = 0;
                    for (auto it = m_inventory.begin(); it != m_inventory.end(); ++it)
                    {
                        if (it->m_itemName == itemName)
                        {
                            ++work;
                        }
                    }

                    if (work >= num)
                    {
                        std::deque<bool> work = m_vecQuestData.at(i).GetStartFlag();
                        work.at(j) = true;
                        m_vecQuestData.at(i).SetStartFlag(work);
                    }
                }
                else if (m_vecQuestData.at(i).GetStartType().at(j) == eStartType::INVENTORY_LEVEL)
                {
                    // 3:トマト:5だったら強化値3のトマトが5個以上あったらの意味
                    std::wstring opt = m_vecQuestData.at(i).GetStartOption1().at(j);
                    std::vector<std::wstring> vs = split(opt, ':');
                    int level = std::stoi(vs.at(0));
                    std::wstring itemName = vs.at(1);
                    int num = std::stoi(vs.at(2));

                    int work = 0;
                    for (auto it = m_inventory.begin(); it != m_inventory.end(); ++it)
                    {
                        if (it->m_itemName == itemName && it->m_level == level)
                        {
                            ++work;
                        }
                    }

                    if (work >= num)
                    {
                        std::deque<bool> work = m_vecQuestData.at(i).GetStartFlag();
                        work.at(j) = true;
                        m_vecQuestData.at(i).SetStartFlag(work);
                    }
                }
            }
        }
    }

    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        if (m_vecQuestData.at(i).GetState() == eQuestState::STARTED ||
            m_vecQuestData.at(i).GetState() == eQuestState::START)
        {
            for (std::size_t j = 0; j < m_vecQuestData.at(i).GetFinishType().size(); ++j)
            {
                if (m_vecQuestData.at(i).GetFinishType().at(j) == eFinishType::INVENTORY)
                {
                    // トマト:5だったらトマトが5個以上あったらの意味
                    std::wstring opt = m_vecQuestData.at(i).GetFinishOption1().at(j);
                    std::vector<std::wstring> vs = split(opt, ':');
                    std::wstring itemName = vs.at(0);
                    int num = std::stoi(vs.at(1));

                    int work = 0;
                    for (auto it = m_inventory.begin(); it != m_inventory.end(); ++it)
                    {
                        if (it->m_itemName == itemName)
                        {
                            ++work;
                        }
                    }

                    if (work >= num)
                    {
                        std::deque<bool> work = m_vecQuestData.at(i).GetFinishFlag();
                        work.at(j) = true;
                        m_vecQuestData.at(i).SetFinishFlag(work);
                    }
                }
                else if (m_vecQuestData.at(i).GetFinishType().at(j) == eFinishType::INVENTORY_LEVEL)
                {
                    // 3:トマト:5だったら強化値3のトマトが5個以上あったらの意味
                    std::wstring opt = m_vecQuestData.at(i).GetFinishOption1().at(j);
                    std::vector<std::wstring> vs = split(opt, ':');
                    int level = std::stoi(vs.at(0));
                    std::wstring itemName = vs.at(1);
                    int num = std::stoi(vs.at(2));

                    int work = 0;
                    for (auto it = m_inventory.begin(); it != m_inventory.end(); ++it)
                    {
                        if (it->m_itemName == itemName && it->m_level == level)
                        {
                            ++work;
                        }
                    }

                    if (work >= num)
                    {
                        std::deque<bool> work = m_vecQuestData.at(i).GetFinishFlag();
                        work.at(j) = true;
                        m_vecQuestData.at(i).SetFinishFlag(work);
                    }
                }
            }
        }
    }

    if (update)
    {
        UpdateQuestStatus();
    }
}

void NSQuestSystem::QuestSystem::SetStorehouseContent(const int storehouseId, const std::vector<ItemInfo>& list, const bool update)
{
    m_storehouseMap[storehouseId] = list;

    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        if (m_vecQuestData.at(i).GetState() == eQuestState::NOT_START)
        {
            for (std::size_t j = 0; j < m_vecQuestData.at(i).GetStartType().size(); ++j)
            {
                if (m_vecQuestData.at(i).GetStartType().at(j) == eStartType::STOREHOUSE)
                {
                    // 1:トマト:5だったら倉庫1にトマトが5個以上あったらの意味
                    std::wstring opt = m_vecQuestData.at(i).GetStartOption1().at(j);
                    std::vector<std::wstring> vs = split(opt, ':');

                    int storehouseId2 = std::stoi(vs.at(0));
                    std::wstring itemName = vs.at(1);
                    int num = std::stoi(vs.at(2));

                    if (storehouseId != storehouseId2)
                    {
                        continue;
                    }

                    int work = 0;
                    for (auto it = m_storehouseMap.at(storehouseId).begin();
                         it != m_storehouseMap.at(storehouseId).end(); ++it)
                    {
                        if (it->m_itemName == itemName)
                        {
                            ++work;
                        }
                    }

                    if (work >= num)
                    {
                        std::deque<bool> work = m_vecQuestData.at(i).GetStartFlag();
                        work.at(j) = true;
                        m_vecQuestData.at(i).SetStartFlag(work);
                    }
                }
                else if (m_vecQuestData.at(i).GetStartType().at(j) == eStartType::STOREHOUSE_LEVEL)
                {
                    // 3:トマト:5だったら強化値3のトマトが5個以上あったらの意味
                    std::wstring opt = m_vecQuestData.at(i).GetStartOption1().at(j);
                    std::vector<std::wstring> vs = split(opt, ':');

                    int storehouseId2 = std::stoi(vs.at(0));
                    int level = std::stoi(vs.at(1));
                    std::wstring itemName = vs.at(2);
                    int num = std::stoi(vs.at(3));

                    if (storehouseId != storehouseId2)
                    {
                        continue;
                    }

                    int work = 0;
                    for (auto it = m_storehouseMap.at(storehouseId).begin();
                         it != m_storehouseMap.at(storehouseId).end(); ++it)
                    {
                        if (it->m_itemName == itemName && it->m_level == level)
                        {
                            ++work;
                        }
                    }

                    if (work >= num)
                    {
                        std::deque<bool> work = m_vecQuestData.at(i).GetStartFlag();
                        work.at(j) = true;
                        m_vecQuestData.at(i).SetStartFlag(work);
                    }
                }
            }
        }
    }

    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        if (m_vecQuestData.at(i).GetState() == eQuestState::STARTED ||
            m_vecQuestData.at(i).GetState() == eQuestState::START)
        {
            for (std::size_t j = 0; j < m_vecQuestData.at(i).GetFinishType().size(); ++j)
            {
                if (m_vecQuestData.at(i).GetFinishType().at(j) == eFinishType::STOREHOUSE)
                {
                    // 1:トマト:5だったら倉庫1にトマトが5個以上あったらの意味
                    // （倉庫はゲーム内に複数存在する）
                    std::wstring opt = m_vecQuestData.at(i).GetFinishOption1().at(j);
                    std::vector<std::wstring> vs = split(opt, ':');

                    int storehouseId2 = std::stoi(vs.at(0));
                    std::wstring itemName = vs.at(1);
                    int num = std::stoi(vs.at(2));

                    if (storehouseId != storehouseId2)
                    {
                        continue;
                    }

                    int work = 0;
                    for (auto it = m_storehouseMap.at(storehouseId).begin();
                         it != m_storehouseMap.at(storehouseId).end(); ++it)
                    {
                        if (it->m_itemName == itemName)
                        {
                            ++work;
                        }
                    }

                    if (work >= num)
                    {
                        std::deque<bool> work = m_vecQuestData.at(i).GetFinishFlag();
                        work.at(j) = true;
                        m_vecQuestData.at(i).SetFinishFlag(work);
                    }
                }
                else if (m_vecQuestData.at(i).GetFinishType().at(j) == eFinishType::STOREHOUSE_LEVEL)
                {
                    // 1:2:トマト:5だったら倉庫1に強化値＋２のトマトが5個以上あったらの意味
                    std::wstring opt = m_vecQuestData.at(i).GetFinishOption1().at(j);
                    std::vector<std::wstring> vs = split(opt, ':');

                    int storehouseId2 = std::stoi(vs.at(0));
                    int level = std::stoi(vs.at(1));
                    std::wstring itemName = vs.at(2);
                    int num = std::stoi(vs.at(3));

                    if (storehouseId != storehouseId2)
                    {
                        continue;
                    }

                    int work = 0;
                    for (auto it = m_storehouseMap.at(storehouseId).begin();
                         it != m_storehouseMap.at(storehouseId).end(); ++it)
                    {
                        if (it->m_itemName == itemName && it->m_level == level)
                        {
                            ++work;
                        }
                    }

                    if (work >= num)
                    {
                        std::deque<bool> work = m_vecQuestData.at(i).GetFinishFlag();
                        work.at(j) = true;
                        m_vecQuestData.at(i).SetFinishFlag(work);
                    }
                }
            }
        }
    }

    if (update)
    {
        UpdateQuestStatus();
    }
}

void NSQuestSystem::QuestSystem::SetBodyStamina(const int stamina)
{
    m_bodyStamina = stamina;

    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        if (m_vecQuestData.at(i).GetState() == eQuestState::NOT_START)
        {
            for (std::size_t j = 0; j < m_vecQuestData.at(i).GetStartType().size(); ++j)
            {
                if (m_vecQuestData.at(i).GetStartType().at(j) == eStartType::BODY_STAMINA_LESS)
                {
                    std::wstring opt = m_vecQuestData.at(i).GetStartOption1().at(j);
                    int stamina = std::stoi(opt);
                    if (m_bodyStamina < stamina)
                    {
                        std::deque<bool> work = m_vecQuestData.at(i).GetStartFlag();
                        work.at(j) = true;
                        m_vecQuestData.at(i).SetStartFlag(work);
                    }
                }
            }
        }
    }

    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        if (m_vecQuestData.at(i).GetState() == eQuestState::STARTED ||
            m_vecQuestData.at(i).GetState() == eQuestState::START)
        {
            for (std::size_t j = 0; j < m_vecQuestData.at(i).GetFinishType().size(); ++j)
            {
                if (m_vecQuestData.at(i).GetFinishType().at(j) == eFinishType::BODY_STAMINA_LESS)
                {
                    std::wstring opt = m_vecQuestData.at(i).GetFinishOption1().at(j);
                    int stamina = std::stoi(opt);
                    if (m_bodyStamina < stamina)
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

void NSQuestSystem::QuestSystem::SetBrainStamina(const int stamina)
{
    m_brainStamina = stamina;

    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        if (m_vecQuestData.at(i).GetState() == eQuestState::NOT_START)
        {
            for (std::size_t j = 0; j < m_vecQuestData.at(i).GetStartType().size(); ++j)
            {
                if (m_vecQuestData.at(i).GetStartType().at(j) == eStartType::BRAIN_STAMINA_LESS)
                {
                    std::wstring opt = m_vecQuestData.at(i).GetStartOption1().at(j);
                    int stamina = std::stoi(opt);
                    if (m_brainStamina < stamina)
                    {
                        std::deque<bool> work = m_vecQuestData.at(i).GetStartFlag();
                        work.at(j) = true;
                        m_vecQuestData.at(i).SetStartFlag(work);
                    }
                }
            }
        }
    }

    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        if (m_vecQuestData.at(i).GetState() == eQuestState::STARTED ||
            m_vecQuestData.at(i).GetState() == eQuestState::START)
        {
            for (std::size_t j = 0; j < m_vecQuestData.at(i).GetFinishType().size(); ++j)
            {
                if (m_vecQuestData.at(i).GetFinishType().at(j) == eFinishType::BRAIN_STAMINA_LESS)
                {
                    std::wstring opt = m_vecQuestData.at(i).GetFinishOption1().at(j);
                    int stamina = std::stoi(opt);
                    if (m_brainStamina < stamina)
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

std::wstring NSQuestSystem::QuestSystem::GetQuestIdStartByExamine(const float x,
                                                                 const float y,
                                                                 const float z)
{
    std::wstring result;
    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        if (m_vecQuestData.at(i).GetState() == eQuestState::NOT_START)
        {
            for (std::size_t j = 0; j < m_vecQuestData.at(i).GetStartType().size(); ++j)
            {
                if (m_vecQuestData.at(i).GetStartType().at(j) == eStartType::EXAMINE)
                {
                    // 0.0:1.0:2.0:3.0だったら座標(0.0, 1.0, 2.0)で半径が3.0、の意味
                    std::wstring xyzr = m_vecQuestData.at(i).GetStartOption1().at(j);
                    std::vector<std::wstring> vs = split(xyzr, ':');
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

std::wstring NSQuestSystem::QuestSystem::GetQuestIdFinishByExamine(const float x,
                                                                  const float y,
                                                                  const float z)
{
    std::wstring result;
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
                    std::wstring xyzr = m_vecQuestData.at(i).GetFinishOption1().at(j);
                    std::vector<std::wstring> vs = split(xyzr, ':');
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

void NSQuestSystem::QuestSystem::SetQuestFinish(const std::wstring& id)
{
    auto it = std::find_if(m_vecQuestData.begin(), m_vecQuestData.end(),
                           [&](const QuestData& x)
                           {
                               return x.GetId() == id;
                           });

    it->SetState(eQuestState::FINISHED);
}

void NSQuestSystem::QuestSystem::SetCurrentDateTime(const int year, const int month, const int day, const int hour, const int minute, const int second, const bool update)
{
    m_currentYear = year;
    m_currentMonth = month;
    m_currentDay = day;
    m_currentHour = hour;
    m_currentMinute = minute;
    m_currentSecond = second;

    // 時間経過で完了するクエストがあるか
    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        // 開始済みのクエストの完了フラグが全部trueならクエスト完了とする
        if (m_vecQuestData.at(i).GetState() == eQuestState::STARTED ||
            m_vecQuestData.at(i).GetState() == eQuestState::START)
        {
            for (std::size_t j = 0; j < m_vecQuestData.at(i).GetFinishType().size(); ++j)
            {
                if (m_vecQuestData.at(i).GetFinishType().at(j) == eFinishType::TIME_PAST)
                {
                    // 0:0:1:2:3:4だったら1日と2時間と3分と4秒経過したら完了、の意味
                    // 1月1日0時0分0秒に開始したクエストは1月2日2時3分4秒以降に完了となる。
                    // 12月31日23時59分59秒に開始したクエストは1月2日2時3分3秒以降に完了となる。
                    // 1月25日に1か月と10日後に完了するクエストを開始したら、完了するのは3月7日（2月は28日しかないことに注意））
                    // やっかい。

                    // クエストの完了時刻を求める
                    int finishYear = 0;
                    int finishMonth = 0;
                    int finishDay = 0;
                    int finishHour = 0;
                    int finishMinute = 0;
                    int finishSecond = 0;
                    {
                        int startYear = 0;
                        int startMonth = 0;
                        int startDay = 0;
                        int startHour = 0;
                        int startMinute = 0;
                        int startSecond = 0;

                        m_vecQuestData.at(i).GetStartDateTime(&startYear, &startMonth, &startDay,
                                                              &startHour, &startMinute, &startSecond);

                        std::wstring datetime = m_vecQuestData.at(i).GetFinishOption1().at(j);
                        std::vector<std::wstring> work = split(datetime, ':');

                        int timeLimitYear = std::stoi(work.at(0));
                        int timeLimitMonth = std::stoi(work.at(1));
                        int timeLimitDay = std::stoi(work.at(2));
                        int timeLimitHour = std::stoi(work.at(3));
                        int timeLimitMinute = std::stoi(work.at(4));
                        int timeLimitSecond = std::stoi(work.at(5));


                        finishYear = startYear + timeLimitYear;
                        finishMonth = startMonth + timeLimitMonth;
                        finishDay = startDay + timeLimitDay;
                        finishHour = startHour + timeLimitHour;
                        finishMinute = startMinute + timeLimitMinute;
                        finishSecond = startSecond + timeLimitSecond;

                        if (finishSecond >= 60)
                        {
                            finishSecond -= 60;
                            finishMinute++;
                        }

                        if (finishMinute >= 60)
                        {
                            finishMinute -= 60;
                            finishHour++;
                        }

                        if (finishHour >= 24)
                        {
                            finishHour -= 24;
                            finishDay++;
                        }

                        // 0月というのはない、はず・・・
                        int dayCount[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
                        if (finishDay >= dayCount[finishMonth])
                        {
                            finishDay -= dayCount[finishMonth];
                            finishMonth++;
                        }

                        if (finishMonth >= 13)
                        {
                            finishMonth -= 12;
                            finishYear++;
                        }
                    }

                    // 完了時刻を過ぎているか？
                    bool bFinish = false;
                    {
                        if (finishYear < m_currentYear)
                        {
                            bFinish = true;
                        }
                        else if (finishYear > m_currentYear)
                        {
                            bFinish = false;
                        }
                        else if (finishYear == m_currentYear)
                        {
                            if (finishMonth < m_currentMonth)
                            {
                                bFinish = true;
                            }
                            else if (finishMonth > m_currentMonth)
                            {
                                bFinish = false;
                            }
                            else if (finishMonth == m_currentMonth)
                            {
                                if (finishDay < m_currentDay)
                                {
                                    bFinish = true;
                                }
                                else if (finishDay > m_currentDay)
                                {
                                    bFinish = false;
                                }
                                else if (finishDay == m_currentDay)
                                {
                                    if (finishHour < m_currentHour)
                                    {
                                        bFinish = true;
                                    }
                                    else if (finishHour > m_currentHour)
                                    {
                                        bFinish = false;
                                    }
                                    else if (finishHour == m_currentHour)
                                    {
                                        if (finishMinute < m_currentMinute)
                                        {
                                            bFinish = true;
                                        }
                                        else if (finishMinute > m_currentMinute)
                                        {
                                            bFinish = false;
                                        }
                                        else if (finishMinute == m_currentMinute)
                                        {
                                            if (finishSecond < m_currentSecond)
                                            {
                                                bFinish = true;
                                            }
                                            else if (finishSecond > m_currentSecond)
                                            {
                                                bFinish = false;
                                            }
                                            else if (finishSecond == m_currentSecond)
                                            {
                                                bFinish = true;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                    if (bFinish)
                    {
                        std::deque<bool> work = m_vecQuestData.at(i).GetFinishFlag();
                        work.at(j) = true;
                        m_vecQuestData.at(i).SetFinishFlag(work);
                    }
                }
            }
        }
    }

    // 夜だったら/朝だったら完了のクエスト
    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        // 開始済みのクエストの完了フラグが全部trueならクエスト完了とする
        if (m_vecQuestData.at(i).GetState() == eQuestState::STARTED ||
            m_vecQuestData.at(i).GetState() == eQuestState::START)
        {
            for (std::size_t j = 0; j < m_vecQuestData.at(i).GetFinishType().size(); ++j)
            {
                // 6 ~ 18 昼
                // 18 ~ 6 夜
                if (18 <= m_currentHour || m_currentHour < 6)
                {
                    if (m_vecQuestData.at(i).GetFinishType().at(j) == eFinishType::AT_NIGHT)
                    {
                        std::deque<bool> work = m_vecQuestData.at(i).GetFinishFlag();
                        work.at(j) = true;
                        m_vecQuestData.at(i).SetFinishFlag(work);
                    }
                    else if (m_vecQuestData.at(i).GetFinishType().at(j) == eFinishType::AT_DAYTIME)
                    {
                        std::deque<bool> work = m_vecQuestData.at(i).GetFinishFlag();
                        work.at(j) = false;
                        m_vecQuestData.at(i).SetFinishFlag(work);
                    }
                }
                else if (6 <= m_currentHour && m_currentHour < 18)
                {
                    if (m_vecQuestData.at(i).GetFinishType().at(j) == eFinishType::AT_DAYTIME)
                    {
                        std::deque<bool> work = m_vecQuestData.at(i).GetFinishFlag();
                        work.at(j) = true;
                        m_vecQuestData.at(i).SetFinishFlag(work);
                    }
                    else if (m_vecQuestData.at(i).GetFinishType().at(j) == eFinishType::AT_NIGHT)
                    {
                        std::deque<bool> work = m_vecQuestData.at(i).GetFinishFlag();
                        work.at(j) = false;
                        m_vecQuestData.at(i).SetFinishFlag(work);
                    }
                }
            }
        }
    }

    if (update)
    {
        UpdateQuestStatus();
    }
}

void NSQuestSystem::QuestSystem::SetNpcIsAlive(const std::wstring& npcKey, const bool bAlive, const bool update)
{
    m_NpcAlive[npcKey] = bAlive;

    // NPCが生存していたら完了のクエスト
    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        // 開始済みのクエストの完了フラグが全部trueならクエスト完了とする
        if (m_vecQuestData.at(i).GetState() == eQuestState::STARTED ||
            m_vecQuestData.at(i).GetState() == eQuestState::START)
        {
            for (std::size_t j = 0; j < m_vecQuestData.at(i).GetFinishType().size(); ++j)
            {
                if (m_vecQuestData.at(i).GetFinishType().at(j) == eFinishType::NPC_ALIVE)
                {
                    std::wstring npcKey_ = m_vecQuestData.at(i).GetFinishOption1().at(j);

                    if (m_NpcAlive.find(npcKey_) != m_NpcAlive.end())
                    {
                        if (m_NpcAlive[npcKey_])
                        {
                            std::deque<bool> work = m_vecQuestData.at(i).GetFinishFlag();
                            work.at(j) = true;
                            m_vecQuestData.at(i).SetFinishFlag(work);
                        }
                        else
                        {
                            std::deque<bool> work = m_vecQuestData.at(i).GetFinishFlag();
                            work.at(j) = false;
                            m_vecQuestData.at(i).SetFinishFlag(work);
                        }
                    }
                }
            }
        }
    }

    if (update)
    {
        UpdateQuestStatus();
    }
}

void NSQuestSystem::QuestSystem::SetRaftNum(const int arg, const bool update)
{
    // イカダの個数で完了のクエスト
    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        // 開始済みのクエストの完了フラグが全部trueならクエスト完了とする
        if (m_vecQuestData.at(i).GetState() == eQuestState::STARTED ||
            m_vecQuestData.at(i).GetState() == eQuestState::START)
        {
            for (std::size_t j = 0; j < m_vecQuestData.at(i).GetFinishType().size(); ++j)
            {
                if (m_vecQuestData.at(i).GetFinishType().at(j) == eFinishType::RAFT_NUM)
                {
                    std::wstring raftNum = m_vecQuestData.at(i).GetFinishOption1().at(j);
                    int nRaftNum = std::stoi(raftNum);
                    if (m_raftNum >= nRaftNum)
                    {
                        std::deque<bool> work = m_vecQuestData.at(i).GetFinishFlag();
                        work.at(j) = true;
                        m_vecQuestData.at(i).SetFinishFlag(work);
                    }
                }
            }
        }
    }

    if (update)
    {
        UpdateQuestStatus();
    }
}


