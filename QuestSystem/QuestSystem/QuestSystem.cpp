#include "QuestSystem.h"
#include <windows.h>
#include <fstream>
#include <sstream>
#include "Shlwapi.h"
#include "CaesarCipher.h"
#include "HeaderOnlyCsv.hpp"

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

�N�G�X�gID,�J�n�^�C�v,�J�n�^�C�v�I�v�V����,�J�n�C�x���g,�����^�C�v,�����I�v�V�����P,�����I�v�V�����Q,�����C�x���g
Q1,�l�Ƙb������,����ɂ���,<speak><����ɂ���>�]���r���R�C�|���Ă���,"�G��|������
�l�Ƙb������","�X���C��
����ɂ���",3,"<speak><����ɂ���>���肪�Ƃ��������܂���
<image>kinniku.png"
Q2,�l�Ƙb������,����ɂ���,<speak><����ɂ���>�]���r�ƃX���C����|���Ă���,"�G��|������
�G��|������
�l�Ƙb������","�X���C��
�]���r
����ɂ���","3
4","<speak><����ɂ���>���肪�Ƃ��������܂���
<image>kinniku.png"
Q3,"�l�Ƙb������
�N�G�X�g���������Ă��Ȃ��Ȃ�","�V���������
Q3",<speak><�V���������>����ɂ���,��������,,,
Q4,"�l�Ƙb������
�N�G�X�g���������Ă�����","�V���������
Q3",<speak><�V���������>�Ȃ񂩂悤�ł���,��������,,,

*/
bool QuestSystem::Init(const std::string& csvFilePath,
                       const std::string& savefile,
                       const bool encrypt)
{
    int result = PathFileExists(csvFilePath.c_str());
    if (result == 0)
    {
        throw std::exception();
    }

    std::stringstream ss;

    if (encrypt == false)
    {
        std::ifstream ifs(csvFilePath);
        ss << ifs.rdbuf();
        ifs.close();
    }
    else
    {
        std::string work = CaesarCipher::DecryptFromFile(csvFilePath);
        ss.str(work);
    }

    std::string buff;
    std::string buffComma;
    QuestData questData;
    int row = 0;
    int col = 0;
    bool doubleQuote = false;
    bool doubleQuoteMode = false;

    while (std::getline(ss, buff))
    {
        // �擪�s�͖���
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
                if (buffComma == "�l�Ƙb������")
                {
                    work.push_back(eStartType::TALK);
                    work2.push_back(false);
                }
                else if (buffComma == "�N�G�X�g���������Ă�����")
                {
                    work.push_back(eStartType::QUEST_FINISHED);
                    work2.push_back(false);
                }
                else if (buffComma == "�N�G�X�g���������Ă��Ȃ��Ȃ�")
                {
                    work.push_back(eStartType::QUEST_NOT_FINISHED);
                    work2.push_back(true);
                }
                else if (buffComma == "�ʒu")
                {
                    work.push_back(eStartType::POS);
                    work2.push_back(false);
                }
                else if (buffComma == "���ׂ���")
                {
                    work.push_back(eStartType::EXAMINE);
                    work2.push_back(false);
                }
                else if (buffComma == "��ł��N�G�X�g���������Ă�����")
                {
                    work.push_back(eStartType::QUEST_FINISH_OR);
                    work2.push_back(false);
                }
                else if (buffComma == "�C���x���g����X��Y��������")
                {
                    work.push_back(eStartType::INVENTORY);
                    work2.push_back(false);
                }
                else if (buffComma == "�q��A��X��Y��������")
                {
                    work.push_back(eStartType::STOREHOUSE);
                    work2.push_back(false);
                }
                else if (buffComma == "�C���x���g���ɋ����lX��Y��Z��������")
                {
                    work.push_back(eStartType::INVENTORY_LEVEL);
                    work2.push_back(false);
                }
                else if (buffComma == "�q��A�ɋ����lX��Y��Z��������")
                {
                    work.push_back(eStartType::STOREHOUSE_LEVEL);
                    work2.push_back(false);
                }
                else if (buffComma == "�̗̂̑͂�X�ȉ���������")
                {
                    work.push_back(eStartType::BODY_STAMINA_LESS);
                    work2.push_back(false);
                }
                else if (buffComma == "�]�̗̑͂�X�ȉ���������")
                {
                    work.push_back(eStartType::BRAIN_STAMINA_LESS);
                    work2.push_back(false);
                }
                else if (buffComma == "�ʒu���͈͊O")
                {
                    work.push_back(eStartType::POS_OUT);
                    work2.push_back(false);
                }
                else
                {
                    throw std::exception(buffComma.c_str());
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
                if (buffComma == "�G��|������")
                {
                    work.push_back(eFinishType::DEFEAT_ENEMY);
                    work2.push_back(false);
                }
                else if (buffComma == "�l�Ƙb������")
                {
                    work.push_back(eFinishType::TALK);
                    work2.push_back(false);
                }
                else if (buffComma == "��������")
                {
                    work.push_back(eFinishType::AUTO);
                    work2.push_back(true);
                }
                else if (buffComma == "�ʒu")
                {
                    work.push_back(eFinishType::POS);
                    work2.push_back(false);
                }
                else if (buffComma == "�N�G�X�g���������Ă�����")
                {
                    work.push_back(eFinishType::QUEST_FINISHED);
                    work2.push_back(false);
                }
                else if (buffComma == "���ׂ���")
                {
                    work.push_back(eFinishType::EXAMINE);
                    work2.push_back(false);
                }
                else if (buffComma == "�C���x���g����X��Y��������")
                {
                    work.push_back(eFinishType::INVENTORY);
                    work2.push_back(false);
                }
                else if (buffComma == "�q��A��X��Y��������")
                {
                    work.push_back(eFinishType::STOREHOUSE);
                    work2.push_back(false);
                }
                else if (buffComma == "�C���x���g���ɋ����lX��Y��Z��������")
                {
                    work.push_back(eFinishType::INVENTORY_LEVEL);
                    work2.push_back(false);
                }
                else if (buffComma == "�q��A�ɋ����lX��Y��Z��������")
                {
                    work.push_back(eFinishType::STOREHOUSE_LEVEL);
                    work2.push_back(false);
                }
                else if (buffComma == "�̗̂̑͂�X�ȉ���������")
                {
                    work.push_back(eFinishType::BODY_STAMINA_LESS);
                    work2.push_back(false);
                }
                else if (buffComma == "�]�̗̑͂�X�ȉ���������")
                {
                    work.push_back(eFinishType::BRAIN_STAMINA_LESS);
                    work2.push_back(false);
                }
                else if (buffComma == "�ʒu���͈͊O")
                {
                    work.push_back(eFinishType::POS_OUT);
                    work2.push_back(false);
                }
                else if (buffComma == "���Ԃ��o�߂�����")
                {
                    work.push_back(eFinishType::TIME_PAST);
                    work2.push_back(false);
                }
                else if (buffComma == "�邾������")
                {
                    work.push_back(eFinishType::AT_NIGHT);
                    work2.push_back(false);
                }
                else if (buffComma == "����������")
                {
                    work.push_back(eFinishType::AT_DAYTIME);
                    work2.push_back(false);
                }
                else
                {
                    throw std::exception(buffComma.c_str());
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
                work2[(int)work.size() - 1] = work3;
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
        // �ʏ�̃N�G�X�g�f�[�^�͏�L�̂悤�ȃf�[�^�ł��邪
        // 0,1,2,3,4,5,6,
        // ��L�̂悤�ɍŌ�̗�ɉ����Ȃ���������B
        // ���̂Ƃ�getline�֐���'6'�܂ł����ǂ߂Ȃ��B
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

    // �Z�[�u�f�[�^��ǂ�
    {
        std::vector<std::vector<std::string>> vvs;
        if (encrypt == false)
        {
            vvs = csv::Read(savefile);
        }
        else
        {
            std::string work = CaesarCipher::DecryptFromFile(savefile);
            csv::ReadFromString(work);
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

            if (vvs.at(i).at(1) == "NOT_START")
            {
                it->SetState(eQuestState::NOT_START);
            }
            else if (vvs.at(i).at(1) == "START")
            {
                it->SetState(eQuestState::START);
            }
            else if (vvs.at(i).at(1) == "STARTED")
            {
                it->SetState(eQuestState::STARTED);
            }
            else if (vvs.at(i).at(1) == "FINISH")
            {
                it->SetState(eQuestState::FINISH);
            }
            else if (vvs.at(i).at(1) == "FINISHED")
            {
                it->SetState(eQuestState::FINISHED);
            }

            {
                std::string work = vvs.at(i).at(2);
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

void NSQuestSystem::QuestSystem::Save(const std::string& filename, const bool encrypt)
{
    std::vector<std::vector<std::string>> vvs;
    std::vector<std::string> vs;
    vs.push_back("�N�G�X�gID");
    vs.push_back("��");
    vs.push_back("�N�G�X�g�J�n����");
    vvs.push_back(vs);
    vs.clear();

    for (size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        std::string work;
        work = m_vecQuestData.at(i).GetId();
        vs.push_back(work);

        if (m_vecQuestData.at(i).GetState() == eQuestState::NOT_START)
        {
            vs.push_back("NOT_START");
        }
        else if (m_vecQuestData.at(i).GetState() == eQuestState::START)
        {
            vs.push_back("START");
        }
        else if (m_vecQuestData.at(i).GetState() == eQuestState::STARTED)
        {
            vs.push_back("STARTED");
        }
        else if (m_vecQuestData.at(i).GetState() == eQuestState::FINISH)
        {
            vs.push_back("FINISH");
        }
        else if (m_vecQuestData.at(i).GetState() == eQuestState::FINISHED)
        {
            vs.push_back("FINISHED");
        }

        {
            int startYear = 0;
            int startMonth = 0;
            int startDay = 0;
            int startHour = 0;
            int startMinute = 0;
            int startSecond = 0;

            m_vecQuestData.at(i).GetStartDateTime(&startYear, &startMonth, &startDay, &startHour, &startMinute, &startSecond);

            std::string year = std::to_string(startYear);
            std::string month = std::to_string(startMonth);
            std::string day = std::to_string(startDay);
            std::string hour = std::to_string(startHour);
            std::string minute = std::to_string(startMinute);
            std::string second = std::to_string(startSecond);

            std::string work;
            work = year + ":";
            work += month + ":";
            work += day + ":";
            work += hour + ":";
            work += minute + ":";
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
        std::stringstream ss;
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

    // �b�������邱�Ƃ̓N�G�X�g�̊J�n�����ł�����A�I�������ł�����
    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        // �J�n�ς݂̃N�G�X�g�̊����t���O���S��true�Ȃ�N�G�X�g�����Ƃ���
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
                    // 0.0:1.0:2.0:3.0����������W(0.0, 1.0, 2.0)�Ŕ��a��3.0�A�̈Ӗ�
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

                    if (m_vecQuestData.at(i).GetStartType().at(j) == eStartType::POS)
                    {
                        if (r <= startR)
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
                        if (r >= startR)
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

    // ���W�ɓ��B�������Ƃ̓N�G�X�g�̊J�n�����ł�����A�I�������ł�����
    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        // �J�n�ς݂̃N�G�X�g�̊����t���O���S��true�Ȃ�N�G�X�g�����Ƃ���
        if (m_vecQuestData.at(i).GetState() == eQuestState::STARTED ||
            m_vecQuestData.at(i).GetState() == eQuestState::START)
        {
            for (std::size_t j = 0; j < m_vecQuestData.at(i).GetFinishType().size(); ++j)
            {
                if (m_vecQuestData.at(i).GetFinishType().at(j) == eFinishType::POS ||
                    m_vecQuestData.at(i).GetFinishType().at(j) == eFinishType::POS_OUT)
                {
                    // 0.0:1.0:2.0:3.0����������W(0.0, 1.0, 2.0)�Ŕ��a��3.0�A�̈Ӗ�
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

                    if (m_vecQuestData.at(i).GetFinishType().at(j) == eFinishType::POS)
                    {
                        if (r <= startR)
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
                        if (r >= startR)
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

void QuestSystem::UpdateQuestStatus()
{
    // �N�G�X�g�J�n�`�F�b�N
    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        // �S��true�Ȃ�N�G�X�g�J�n�Ƃ���
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

    // �N�G�X�g�����`�F�b�N
    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        // �J�n�ς݂̃N�G�X�g�̊����t���O���S��true�Ȃ�N�G�X�g�����Ƃ���
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

    // �N�G�X�g�̊J�n�����u�N�G�X�g���������Ă�����v�̏���
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

    // �N�G�X�g�̊J�n�����u�N�G�X�g���������Ă��Ȃ��Ȃ�v�̏���
    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        if (m_vecQuestData.at(i).GetState() == eQuestState::NOT_START)
        {
            std::vector<eStartType> startType = m_vecQuestData.at(i).GetStartType();
            for (std::size_t j = 0; j < startType.size(); ++j)
            {
                if (startType.at(j) == eStartType::QUEST_NOT_FINISHED)
                {
                    std::vector<std::string> vs = m_vecQuestData.at(i).GetStartOption1();
                    std::string questId = vs.at(j);
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

    // �N�G�X�g�̊J�n�����u��ł��N�G�X�g���������Ă�����v�̏���
    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        if (m_vecQuestData.at(i).GetState() == eQuestState::NOT_START)
        {
            std::vector<eStartType> startType = m_vecQuestData.at(i).GetStartType();
            for (std::size_t j = 0; j < startType.size(); ++j)
            {
                if (startType.at(j) == eStartType::QUEST_FINISH_OR)
                {
                    std::vector<std::string> vs = m_vecQuestData.at(i).GetStartOption1();
                    std::vector<std::string> questIds = split(vs.at(j), ':');
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

    // �N�G�X�g�̊��������u�N�G�X�g���������Ă�����v�̏���
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

    // �N�G�X�g�̊J�n�����u�N�G�X�g���������Ă�����v�A
    // �N�G�X�g�̊J�n�����u�N�G�X�g���������Ă��Ȃ��Ȃ�v�A
    // �N�G�X�g�̊J�n�����u��ł��N�G�X�g���������Ă�����v�A
    // �N�G�X�g�̊��������u�N�G�X�g���������Ă�����v�̃`�F�b�N���s����������
    // ���߂āA�N�G�X�g�J�n�E�����`�F�b�N
 
    // �N�G�X�g�J�n�`�F�b�N
    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        // �S��true�Ȃ�N�G�X�g�J�n�Ƃ���
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

    // �N�G�X�g�����`�F�b�N
    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        // �J�n�ς݂̃N�G�X�g�̊����t���O���S��true�Ȃ�N�G�X�g�����Ƃ���
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
                    // 0.0:1.0:2.0:3.0����������W(0.0, 1.0, 2.0)�Ŕ��a��3.0�A�̈Ӗ�
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

    // ���W�ɓ��B�������Ƃ̓N�G�X�g�̊J�n�����ł�����A�I�������ł�����
    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        // �J�n�ς݂̃N�G�X�g�̊����t���O���S��true�Ȃ�N�G�X�g�����Ƃ���
        if (m_vecQuestData.at(i).GetState() == eQuestState::STARTED ||
            m_vecQuestData.at(i).GetState() == eQuestState::START)
        {
            for (std::size_t j = 0; j < m_vecQuestData.at(i).GetFinishType().size(); ++j)
            {
                if (m_vecQuestData.at(i).GetFinishType().at(j) == eFinishType::EXAMINE)
                {
                    // 0.0:1.0:2.0:3.0����������W(0.0, 1.0, 2.0)�Ŕ��a��3.0�A�̈Ӗ�
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
                    // �g�}�g:5��������g�}�g��5�ȏ゠������̈Ӗ�
                    std::string opt = m_vecQuestData.at(i).GetStartOption1().at(j);
                    std::vector<std::string> vs = split(opt, ':');
                    std::string itemName = vs.at(0);
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
                    // 3:�g�}�g:5�������狭���l3�̃g�}�g��5�ȏ゠������̈Ӗ�
                    std::string opt = m_vecQuestData.at(i).GetStartOption1().at(j);
                    std::vector<std::string> vs = split(opt, ':');
                    int level = std::stoi(vs.at(0));
                    std::string itemName = vs.at(1);
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
                    // �g�}�g:5��������g�}�g��5�ȏ゠������̈Ӗ�
                    std::string opt = m_vecQuestData.at(i).GetFinishOption1().at(j);
                    std::vector<std::string> vs = split(opt, ':');
                    std::string itemName = vs.at(0);
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
                    // 3:�g�}�g:5�������狭���l3�̃g�}�g��5�ȏ゠������̈Ӗ�
                    std::string opt = m_vecQuestData.at(i).GetFinishOption1().at(j);
                    std::vector<std::string> vs = split(opt, ':');
                    int level = std::stoi(vs.at(0));
                    std::string itemName = vs.at(1);
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
                    // 1:�g�}�g:5��������q��1�Ƀg�}�g��5�ȏ゠������̈Ӗ�
                    std::string opt = m_vecQuestData.at(i).GetStartOption1().at(j);
                    std::vector<std::string> vs = split(opt, ':');

                    int storehouseId2 = std::stoi(vs.at(0));
                    std::string itemName = vs.at(1);
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
                    // 3:�g�}�g:5�������狭���l3�̃g�}�g��5�ȏ゠������̈Ӗ�
                    std::string opt = m_vecQuestData.at(i).GetStartOption1().at(j);
                    std::vector<std::string> vs = split(opt, ':');

                    int storehouseId2 = std::stoi(vs.at(0));
                    int level = std::stoi(vs.at(1));
                    std::string itemName = vs.at(2);
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
                    // 1:�g�}�g:5��������q��1�Ƀg�}�g��5�ȏ゠������̈Ӗ�
                    // �i�q�ɂ̓Q�[�����ɕ������݂���j
                    std::string opt = m_vecQuestData.at(i).GetFinishOption1().at(j);
                    std::vector<std::string> vs = split(opt, ':');

                    int storehouseId2 = std::stoi(vs.at(0));
                    std::string itemName = vs.at(1);
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
                    // 1:2:�g�}�g:5��������q��1�ɋ����l�{�Q�̃g�}�g��5�ȏ゠������̈Ӗ�
                    std::string opt = m_vecQuestData.at(i).GetFinishOption1().at(j);
                    std::vector<std::string> vs = split(opt, ':');

                    int storehouseId2 = std::stoi(vs.at(0));
                    int level = std::stoi(vs.at(1));
                    std::string itemName = vs.at(2);
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
                    std::string opt = m_vecQuestData.at(i).GetStartOption1().at(j);
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
                    std::string opt = m_vecQuestData.at(i).GetFinishOption1().at(j);
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
                    std::string opt = m_vecQuestData.at(i).GetStartOption1().at(j);
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
                    std::string opt = m_vecQuestData.at(i).GetFinishOption1().at(j);
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
                    // 0.0:1.0:2.0:3.0����������W(0.0, 1.0, 2.0)�Ŕ��a��3.0�A�̈Ӗ�
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
        // �J�n�ς݂̃N�G�X�g�̊����t���O���S��true�Ȃ�N�G�X�g�����Ƃ���
        if (m_vecQuestData.at(i).GetState() == eQuestState::STARTED ||
            m_vecQuestData.at(i).GetState() == eQuestState::START)
        {
            for (std::size_t j = 0; j < m_vecQuestData.at(i).GetFinishType().size(); ++j)
            {
                if (m_vecQuestData.at(i).GetFinishType().at(j) == eFinishType::EXAMINE)
                {
                    // 0.0:1.0:2.0:3.0����������W(0.0, 1.0, 2.0)�Ŕ��a��3.0�A�̈Ӗ�
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

void NSQuestSystem::QuestSystem::SetQuestFinish(const std::string& id)
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

    // ���Ԍo�߂Ŋ�������N�G�X�g�����邩
    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        // �J�n�ς݂̃N�G�X�g�̊����t���O���S��true�Ȃ�N�G�X�g�����Ƃ���
        if (m_vecQuestData.at(i).GetState() == eQuestState::STARTED ||
            m_vecQuestData.at(i).GetState() == eQuestState::START)
        {
            for (std::size_t j = 0; j < m_vecQuestData.at(i).GetFinishType().size(); ++j)
            {
                if (m_vecQuestData.at(i).GetFinishType().at(j) == eFinishType::TIME_PAST)
                {
                    // 0:0:1:2:3:4��������1����2���Ԃ�3����4�b�o�߂����犮���A�̈Ӗ�
                    // 1��1��0��0��0�b�ɊJ�n�����N�G�X�g��1��2��2��3��4�b�ȍ~�Ɋ����ƂȂ�B
                    // 12��31��23��59��59�b�ɊJ�n�����N�G�X�g��1��2��2��3��3�b�ȍ~�Ɋ����ƂȂ�B
                    // 1��25����1������10����Ɋ�������N�G�X�g���J�n������A��������̂�3��7���i2����28�������Ȃ����Ƃɒ��Ӂj�j
                    // ��������B

                    // �N�G�X�g�̊������������߂�
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

                        std::string datetime = m_vecQuestData.at(i).GetFinishOption1().at(j);
                        std::vector<std::string> work = split(datetime, ':');

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

                        // 0���Ƃ����̂͂Ȃ��A�͂��E�E�E
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

                    // �����������߂��Ă��邩�H
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

    // �邾������/���������犮���̃N�G�X�g
    for (std::size_t i = 0; i < m_vecQuestData.size(); ++i)
    {
        // �J�n�ς݂̃N�G�X�g�̊����t���O���S��true�Ȃ�N�G�X�g�����Ƃ���
        if (m_vecQuestData.at(i).GetState() == eQuestState::STARTED ||
            m_vecQuestData.at(i).GetState() == eQuestState::START)
        {
            for (std::size_t j = 0; j < m_vecQuestData.at(i).GetFinishType().size(); ++j)
            {
                // 6 ~ 18 ��
                // 18 ~ 6 ��
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

std::string NSQuestSystem::QuestSystem::GetHint()
{
    return std::string();
}

