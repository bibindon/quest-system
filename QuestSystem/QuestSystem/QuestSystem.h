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
    void SetStorehouseContent(const std::vector<ItemInfo>& list);

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

private:

    std::vector<QuestData> m_vecQuestData;

    std::vector<ItemInfo> m_inventory;
    std::vector<ItemInfo> m_storehouse;
};
}

