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
    NPC_ALIVE, // NPCが生存していたら
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
    std::wstring m_itemName;
    int m_level = -1;
};

class QuestData
{
public:

    std::wstring GetId() const;
    void SetId(const std::wstring& id);

    std::vector<eStartType> GetStartType();
    void SetStartType(const std::vector<eStartType>& vecStartType);

    std::vector<std::wstring> GetStartOption1();
    void SetStartOption1(const std::vector<std::wstring>&);

    std::vector<std::wstring> GetStartEvent();
    void SetStartEvent(const std::vector<std::wstring>&);

    std::deque<bool> GetStartFlag();
    void SetStartFlag(const std::deque<bool>&);

    std::vector<eFinishType> GetFinishType();
    void SetFinishType(const std::vector<eFinishType>& vecFinishType);

    std::vector<std::wstring> GetFinishOption1();
    void SetFinishOption1(const std::vector<std::wstring>&);

    std::vector<std::wstring> GetFinishOption2();
    void SetFinishOption2(const std::vector<std::wstring>&);

    std::vector<std::wstring> GetFinishEvent();
    void SetFinishEvent(const std::vector<std::wstring>&);

    std::deque<bool> GetFinishFlag();
    void SetFinishFlag(const std::deque<bool>&);

    eQuestState GetState();
    void SetState(const eQuestState state);

    std::unordered_map<int, int> GetCurrentFinishOpt2();
    void SetCurrentFinishOpt2(const std::unordered_map<int, int>&);

    void SetStartDateTime(const int year, const int month, const int day, const int hour, const int minute, const int second);
    void GetStartDateTime(int* year, int* month, int* day, int* hour, int* minute, int* second);

private:

    std::wstring m_id;

    std::vector<eStartType> m_vecStartType;
    std::vector<std::wstring> m_vecStartOption1;
    std::vector<std::wstring> m_vecStartEvent;
    std::deque<bool> m_vecStartFlag;

    std::vector<eFinishType> m_vecFinishType;
    std::vector<std::wstring> m_vecFinishOption1;
    std::vector<std::wstring> m_vecFinishOption2;
    std::vector<std::wstring> m_vecFinishEvent;
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

    bool Init(const std::wstring& csvFilePath,
              const std::wstring& savefile,
              const bool encrypt);

    void Save(const std::wstring& savefile, const bool encrypt);

    //-------------------------------------------------
    // この関数を実行すると、クエストの状態が「START」だったクエストは「STARTED」になる
    //-------------------------------------------------
    std::vector<std::wstring> GetStartQuest();

    std::vector<std::wstring> GetStartedQuest();
    std::vector<std::wstring> GetFinishQuest();

    bool GetQuestFinished(const std::wstring& id);

    void SetTalk(const std::wstring& npc);
    void SetPos(const float x, const float y, const float z, const bool update = true);
    void UpdateQuestStatus();
    void SetDefeatEnemy(const std::wstring& enemy);
    std::vector<std::wstring> GetQuestStartEvent(const std::wstring& id);
    std::vector<std::wstring> GetQuestFinishEvent(const std::wstring& id);
    void SetExamine(const float x, const float y, const float z);

    // インベントリの内容を登録
    void SetInventoryContent(const std::vector<ItemInfo>& list, const bool update = true);

    // 倉庫の内容を登録
    void SetStorehouseContent(const int storehouseId, const std::vector<ItemInfo>& list, const bool update = true);

    void SetBodyStamina(const int stamina);

    void SetBrainStamina(const int stamina);

    // 開始タイプが「調べたら」であるクエストを取得
    // 座標を渡して、その座標で開始するクエストを取得
    // 該当するクエストが複数あっても一つだけ返す
    // まだ開始していないクエストだけが対象であり、
    // 開始済みだったり、完了しているクエストは対象とならない。
    std::wstring GetQuestIdStartByExamine(const float x, const float y, const float z);

    // 完了タイプが「調べたら」なクエストを取得
    // 座標を渡して、その座標で完了するクエストを取得
    // 該当するクエストが複数あっても一つだけ返す
    // 進行中で、まだ完了していないクエストだけが対象であり、
    // 開始していなかったり、完了しているクエストは対象とならない。
    std::wstring GetQuestIdFinishByExamine(const float x, const float y, const float z);

    void SetQuestFinish(const std::wstring& id);

    void SetCurrentDateTime(const int year, const int month, const int day,
                            const int hour, const int minute, const int second,
                            const bool update = true);

    // NPCが生きているか。主に死亡したときに使う。
    void SetNpcIsAlive(const std::wstring& npcKey, const bool bAlive, const bool update);

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

    std::unordered_map<std::wstring, bool> m_NpcAlive;

};
}

