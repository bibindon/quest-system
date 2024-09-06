#pragma once
#include <string>
#include <vector>

enum class eStartType
{
	TALK,
	DEFEAT_ENEMY,
	POS,
	QUEST_FINISHED,
	QUEST_NOT_FINISHED,
};

enum class eFinishType
{
	TALK,
	DEFEAT_ENEMY,
	POS,
	QUEST_FINISHED,
	AUTO,
};

enum class eQuestState
{
	NOT_START,
	START,
	STARTED, // àÍìxÇ≈Ç‡STARTÇì`Ç¶ÇΩÇÁSTARTEDÇ…êÿÇËë÷Ç¶ÇÈ
	FINISH,
	FINISHED, // àÍìxÇ≈Ç‡FINISHÇì`Ç¶ÇΩÇÁFINISHEDÇ…êÿÇËë÷Ç¶ÇÈ
};

struct QuestData
{
	std::string m_id;

	std::vector<eStartType> m_vecStartType;
	std::vector<std::string> m_vecStartOption1;
	std::vector<std::string> m_vecStartEvent;
	std::vector<bool> m_vecStartFlag;

	std::vector<eFinishType> m_vecFinishType;
	std::vector<std::string> m_vecFinishOption1;
	std::vector<std::string> m_vecFinishOption2;
	std::vector<std::string> m_vecFinishEvent;
	std::vector<bool> m_vecFinishFlag;

	eQuestState m_eQuestState = eQuestState::NOT_START;
};

class QuestSystem
{
public:

	QuestSystem();
	bool Init(const std::string& csvFilePath);
	std::vector<std::string> GetStartQuest();
	std::vector<std::string> GetFinishQuest();
	void SetTalk(const std::string& npc);
	void UpdateQuestStatus();
	void SetDefeatEnemy(const std::string& enemy);
	std::vector<std::string> GetQuestStartEvent(const std::string& id);
	std::vector<std::string> GetQuestFinishEvent(const std::string& id);

private:

	std::vector<QuestData> m_vecQuestData;

};

