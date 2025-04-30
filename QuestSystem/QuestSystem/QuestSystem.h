#pragma once
#include <string>
#include <vector>
#include <deque>
#include <unordered_map>

namespace NSQuestSystem
{
enum class eStartType
{
    TALK,
    DEFEAT_ENEMY,
    POS,
    QUEST_FINISHED,
    QUEST_NOT_FINISHED,
    EXAMINE,
    QUEST_FINISH_OR, // �N�G�X�g����ł��������Ă�����
    INVENTORY,
    STOREHOUSE,
    INVENTORY_LEVEL, // �����l���`�F�b�N����
    STOREHOUSE_LEVEL, // �����l���`�F�b�N����
    BODY_STAMINA_LESS, // �̗̂̑͂��w�肵���l��菭�Ȃ�������
    BRAIN_STAMINA_LESS, // �]�̗̑͂��w�肵���l��菭�Ȃ�������
    POS_OUT, // �ʒu���w��͈͂̊O�Ȃ�
};

enum class eFinishType
{
    TALK,
    DEFEAT_ENEMY,
    POS,
    QUEST_FINISHED,
    AUTO,
    EXAMINE,
    INVENTORY,
    STOREHOUSE,
    INVENTORY_LEVEL, // �����l���`�F�b�N����
    STOREHOUSE_LEVEL, // �����l���`�F�b�N����
    BODY_STAMINA_LESS, // �̗̂̑͂��w�肵���l��菭�Ȃ�������
    BRAIN_STAMINA_LESS, // �]�̗̑͂��w�肵���l��菭�Ȃ�������
    POS_OUT, // �ʒu���w��͈͂̊O�Ȃ�
    TIME_PAST, // ���Ԃ��o�߂�����
    AT_NIGHT, // �邾������i���łɖ邾������A���������Ă��܂��B�u��ɂȂ�����v����肽���Ȃ�TIME_PAST�Ƒg�ݍ��킹�Ďg���΂悢�j
              // �i��̓I�ɂ�18������6���j
    AT_DAYTIME, // ����������i��̓I�ɂ�6������18���j
};

enum class eQuestState
{
    NOT_START,
    START,
    STARTED, // ��x�ł�START��`������STARTED�ɐ؂�ւ���H
    FINISH,
    FINISHED, // ��x�ł�FINISH��`������FINISHED�ɐ؂�ւ���H
};

struct ItemInfo
{
    std::string m_itemName;
    int m_level = -1;
};

class QuestData
{
public:

    std::string GetId() const;
    void SetId(const std::string& id);

    std::vector<eStartType> GetStartType();
    void SetStartType(const std::vector<eStartType>& vecStartType);

    std::vector<std::string> GetStartOption1();
    void SetStartOption1(const std::vector<std::string>&);

    std::vector<std::string> GetStartEvent();
    void SetStartEvent(const std::vector<std::string>&);

    std::deque<bool> GetStartFlag();
    void SetStartFlag(const std::deque<bool>&);

    std::vector<eFinishType> GetFinishType();
    void SetFinishType(const std::vector<eFinishType>& vecFinishType);

    std::vector<std::string> GetFinishOption1();
    void SetFinishOption1(const std::vector<std::string>&);

    std::vector<std::string> GetFinishOption2();
    void SetFinishOption2(const std::vector<std::string>&);

    std::vector<std::string> GetFinishEvent();
    void SetFinishEvent(const std::vector<std::string>&);

    std::deque<bool> GetFinishFlag();
    void SetFinishFlag(const std::deque<bool>&);

    eQuestState GetState();
    void SetState(const eQuestState state);

    std::unordered_map<int, int> GetCurrentFinishOpt2();
    void SetCurrentFinishOpt2(const std::unordered_map<int, int>&);

    void SetStartDateTime(const int year, const int month, const int day, const int hour, const int minute, const int second);
    void GetStartDateTime(int* year, int* month, int* day, int* hour, int* minute, int* second);

private:

    std::string m_id;

    std::vector<eStartType> m_vecStartType;
    std::vector<std::string> m_vecStartOption1;
    std::vector<std::string> m_vecStartEvent;
    std::deque<bool> m_vecStartFlag;

    std::vector<eFinishType> m_vecFinishType;
    std::vector<std::string> m_vecFinishOption1;
    std::vector<std::string> m_vecFinishOption2;
    std::vector<std::string> m_vecFinishEvent;
    std::deque<bool> m_vecFinishFlag;

    eQuestState m_eQuestState = eQuestState::NOT_START;
    std::unordered_map<int, int> m_currentFinishOption2;

    int m_startYear = 0;
    int m_startMonth = 0;
    int m_startDay = 0;
    int m_startHour = 0;
    int m_startMinute = 0;
    int m_startSecond = 0;
};

class QuestSystem
{
public:

    QuestSystem();

    bool Init(const std::string& csvFilePath,
              const std::string& savefile,
              const bool encrypt);

    void Save(const std::string& savefile, const bool encrypt);

    std::vector<std::string> GetStartQuest();
    std::vector<std::string> GetFinishQuest();
    void SetTalk(const std::string& npc);
    void SetPos(const float x, const float y, const float z);
    void UpdateQuestStatus();
    void SetDefeatEnemy(const std::string& enemy);
    std::vector<std::string> GetQuestStartEvent(const std::string& id);
    std::vector<std::string> GetQuestFinishEvent(const std::string& id);
    void SetExamine(const float x, const float y, const float z);

    // �C���x���g���̓��e��o�^
    void SetInventoryContent(const std::vector<ItemInfo>& list);

    // �q�ɂ̓��e��o�^
    void SetStorehouseContent(const int storehouseId, const std::vector<ItemInfo>& list);

    void SetBodyStamina(const int stamina);

    void SetBrainStamina(const int stamina);

    // �J�n�^�C�v���u���ׂ���v�ł���N�G�X�g���擾
    // ���W��n���āA���̍��W�ŊJ�n����N�G�X�g���擾
    // �Y������N�G�X�g�����������Ă�������Ԃ�
    // �܂��J�n���Ă��Ȃ��N�G�X�g�������Ώۂł���A
    // �J�n�ς݂�������A�������Ă���N�G�X�g�͑ΏۂƂȂ�Ȃ��B
    std::string GetQuestIdStartByExamine(const float x, const float y, const float z);

    // �����^�C�v���u���ׂ���v�ȃN�G�X�g���擾
    // ���W��n���āA���̍��W�Ŋ�������N�G�X�g���擾
    // �Y������N�G�X�g�����������Ă�������Ԃ�
    // �i�s���ŁA�܂��������Ă��Ȃ��N�G�X�g�������Ώۂł���A
    // �J�n���Ă��Ȃ�������A�������Ă���N�G�X�g�͑ΏۂƂȂ�Ȃ��B
    std::string GetQuestIdFinishByExamine(const float x, const float y, const float z);

    void SetQuestFinish(const std::string& id);

    void SetCurrentDateTime(const int year, const int month, const int day,
                            const int hour, const int minute, const int second);

    // �N�G�X�g�̃q���g��������B
    // �i�s���̃N�G�X�g���I��������@���擾�ł���B
    // �ЂƂ܂����ꂾ���B
    std::string GetHint();

private:

    std::vector<QuestData> m_vecQuestData;

    std::vector<ItemInfo> m_inventory;
    
    // �q�ɂ̓Q�[�����ɕ������݂���
    std::unordered_map<int, std::vector<ItemInfo>> m_storehouseMap;

    int m_bodyStamina = 0;
    int m_brainStamina = 0;

    int m_currentYear = 0;
    int m_currentMonth = 0;
    int m_currentDay = 0;
    int m_currentHour = 0;
    int m_currentMinute = 0;
    int m_currentSecond = 0;

};
}

