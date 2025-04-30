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
    BODY_STAMINA_LESS, // 体の体力が指定した値より少なかったら
    BRAIN_STAMINA_LESS, // 脳の体力が指定した値より少なかったら
    POS_OUT, // 位置が指定範囲の外なら
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
    BODY_STAMINA_LESS, // 体の体力が指定した値より少なかったら
    BRAIN_STAMINA_LESS, // 脳の体力が指定した値より少なかったら
    POS_OUT, // 位置が指定範囲の外なら
    TIME_PAST, // 時間が経過したら
    AT_NIGHT, // 夜だったら（すでに夜だったら、即完了してしまう。「夜になったら」をやりたいならTIME_PASTと組み合わせて使えばよい）
              // （具体的には18時から6時）
    AT_DAYTIME, // 昼だったら（具体的には6時から18時）
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

    // インベントリの内容を登録
    void SetInventoryContent(const std::vector<ItemInfo>& list);

    // 倉庫の内容を登録
    void SetStorehouseContent(const int storehouseId, const std::vector<ItemInfo>& list);

    void SetBodyStamina(const int stamina);

    void SetBrainStamina(const int stamina);

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

    void SetCurrentDateTime(const int year, const int month, const int day,
                            const int hour, const int minute, const int second);

    // クエストのヒントが得られる。
    // 進行中のクエストを終了する方法が取得できる。
    // ひとまずそれだけ。
    std::string GetHint();

private:

    std::vector<QuestData> m_vecQuestData;

    std::vector<ItemInfo> m_inventory;
    
    // 倉庫はゲーム内に複数存在する
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

