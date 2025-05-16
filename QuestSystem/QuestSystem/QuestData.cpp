#include "QuestSystem.h"

using namespace NSQuestSystem;

std::string QuestData::GetId() const
{
    return m_id;
}

void QuestData::SetId(const std::string& id)
{
    m_id = id;
}

std::vector<eStartType> QuestData::GetStartType()
{
    return m_vecStartType;
}

void QuestData::SetStartType(const std::vector<eStartType>& vecStartType)
{
    m_vecStartType = vecStartType;
}

std::vector<std::string> QuestData::GetStartOption1()
{
    return m_vecStartOption1;
}

void QuestData::SetStartOption1(const std::vector<std::string>& vecStartOption1)
{
    m_vecStartOption1 = vecStartOption1;
}

std::vector<std::string> QuestData::GetStartEvent()
{
    return m_vecStartEvent;
}

void QuestData::SetStartEvent(const std::vector<std::string>& vecStartEvent)
{
    m_vecStartEvent = vecStartEvent;
}

std::deque<bool> QuestData::GetStartFlag()
{
    return m_vecStartFlag;
}

void QuestData::SetStartFlag(const std::deque<bool>& vecStartFlag)
{
    m_vecStartFlag = vecStartFlag;
}

std::vector<eFinishType> QuestData::GetFinishType()
{
    return m_vecFinishType;
}

void QuestData::SetFinishType(const std::vector<eFinishType>& vecFinishType)
{
    m_vecFinishType = vecFinishType;
}

std::vector<std::string> QuestData::GetFinishOption1()
{
    return m_vecFinishOption1;
}

void QuestData::SetFinishOption1(const std::vector<std::string>& vecFinishOption1)
{
    m_vecFinishOption1 = vecFinishOption1;
}

std::vector<std::string> QuestData::GetFinishOption2()
{
    return m_vecFinishOption2;
}

void QuestData::SetFinishOption2(const std::vector<std::string>& vecFinishOption2)
{
    m_vecFinishOption2 = vecFinishOption2;
}

std::vector<std::string> QuestData::GetFinishEvent()
{
    return m_vecFinishEvent;
}

void QuestData::SetFinishEvent(const std::vector<std::string>& vecFinishEvent)
{
    m_vecFinishEvent = vecFinishEvent;
}

std::deque<bool> QuestData::GetFinishFlag()
{
    return m_vecFinishFlag;
}

void QuestData::SetFinishFlag(const std::deque<bool>& vecFinishFlag)
{
    m_vecFinishFlag = vecFinishFlag;
}

eQuestState QuestData::GetState()
{
    return m_eQuestState;
}

void QuestData::SetState(const eQuestState state)
{
    m_eQuestState = state;
}

std::unordered_map<int, int> QuestData::GetCurrentFinishOpt2()
{
    return m_currentFinishOption2;
}

void QuestData::SetCurrentFinishOpt2(const std::unordered_map<int, int>& currentFinishOption2)
{
    m_currentFinishOption2 = currentFinishOption2;
}

void NSQuestSystem::QuestData::SetStartDateTime(const int year, const int month, const int day, const int hour, const int minute, const int second)
{
    m_startYear = year;
    m_startMonth = month;
    m_startDay = day;
    m_startHour = hour;
    m_startMinute = minute;
    m_startSecond = second;
}

void NSQuestSystem::QuestData::GetStartDateTime(int* year, int* month, int* day, int* hour, int* minute, int* second)
{
    *year = m_startYear;
    *month = m_startMonth;
    *day = m_startDay;
    *hour = m_startHour;
    *minute = m_startMinute;
    *second = m_startSecond;
}

