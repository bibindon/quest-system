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

struct QuestData
{
	std::string m_id;

	std::vector<eStartType> m_vecStartType;
	std::vector<std::string> m_vecStartOption1;
	std::vector<std::string> m_vecStartEvent;

	std::vector<eFinishType> m_vecFinishType;
	std::vector<std::string> m_vecFinishOption1;
	std::vector<std::string> m_vecFinishOption2;
	std::vector<std::string> m_vecFinishEvent;
};

class QuestSystem
{
public:

	QuestSystem();
	bool Init(const std::string& csvFilePath);

private:

	std::vector<QuestData> m_vecQuestData;

};

