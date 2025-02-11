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
    QUEST_FINISH_OR, // クエストが一つでも完了していたら
    INVENTORY,
    STOREHOUSE,
    INVENTORY_LEVEL, // 強化値もチェックする
    STOREHOUSE_LEVEL, // 強化値もチェックする
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
    INVENTORY_LEVEL, // 強化値もチェックする
    STOREHOUSE_LEVEL, // 強化値もチェックする
};

enum class eQuestState
{
    NOT_START,
    START,
    STARTED, // 一度でもSTARTを伝えたらSTARTEDに切り替える？
    FINISH,
    FINISHED, // 一度でもFINISHを伝えたらFINISHEDに切り替える？
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

    // インベントリの内容を登録
    void SetInventoryContent(const std::vector<ItemInfo>& list);

    // 倉庫の内容を登録
    void SetStorehouseContent(const std::vector<ItemInfo>& list);

    // 開始タイプが「調べたら」であるクエストを取得
    // 座標を渡して、その座標で開始するクエストを取得
    // 該当するクエストが複数あっても一つだけ返す
    // まだ開始していないクエストだけが対象であり、
    // 開始済みだったり、完了しているクエストは対象とならない。
    std::string GetQuestIdStartByExamine(const float x, const float y, const float z);

    // 完了タイプが「調べたら」なクエストを取得
    // 座標を渡して、その座標で完了するクエストを取得
    // 該当するクエストが複数あっても一つだけ返す
    // 進行中で、まだ完了していないクエストだけが対象であり、
    // 開始していなかったり、完了しているクエストは対象とならない。
    std::string GetQuestIdFinishByExamine(const float x, const float y, const float z);

    void SetQuestFinish(const std::string& id);

private:

    std::vector<QuestData> m_vecQuestData;

    std::vector<ItemInfo> m_inventory;
    std::vector<ItemInfo> m_storehouse;
};
}

