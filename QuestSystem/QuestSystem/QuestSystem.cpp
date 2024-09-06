#include "QuestSystem.h"
#include <windows.h>
#include <fstream>
#include <sstream>
#include "Shlwapi.h"

#pragma comment( lib, "Shlwapi.lib" ) 

QuestSystem::QuestSystem()
{
}

void ltrim(std::string& s)
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(),
		[](unsigned char ch)
		{
			return !std::isspace(ch);
		}
	));
}

void rtrim(std::string& s)
{
	s.erase(std::find_if(s.rbegin(), s.rend(),
		[](unsigned char ch)
		{
			return !std::isspace(ch);
		}
	).base(), s.end());
}

void trim(std::string& s)
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
bool QuestSystem::Init(const std::string& csvFilePath)
{
	int result = PathFileExists(csvFilePath.c_str());
	if (result == 0)
	{
		return false;
	}

	std::ifstream ifs(csvFilePath);
	std::string buff;
	std::string buffComma;
	QuestData questData;
	int row = 0;
	int col = 0;
	bool doubleQuote = false;
	bool doubleQuoteMode = false;

	while (std::getline(ifs, buff))
	{
		// 先頭行は無視
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
				questData.m_id = buffComma;
			}
			else if (col == 1)
			{
				if (buffComma == "人と話したら")
				{
					questData.m_vecStartType.push_back(eStartType::TALK);
				}
				else if (buffComma == "クエストが完了していたら")
				{
					questData.m_vecStartType.push_back(eStartType::QUEST_FINISHED);
				}
				else if (buffComma == "クエストが完了していないなら")
				{
					questData.m_vecStartType.push_back(eStartType::QUEST_NOT_FINISHED);
				}
			}
			else if (col == 2)
			{
				questData.m_vecStartOption1.push_back(buffComma);
			}
			else if (col == 3)
			{
				questData.m_vecStartEvent.push_back(buffComma);
			}
			else if (col == 4)
			{
				if (buffComma == "敵を倒したら")
				{
					questData.m_vecFinishType.push_back(eFinishType::DEFEAT_ENEMY);
				}
				else if (buffComma == "人と話したら")
				{
					questData.m_vecFinishType.push_back(eFinishType::TALK);
				}
				else if (buffComma == "自動完了")
				{
					questData.m_vecFinishType.push_back(eFinishType::AUTO);
				}
			}
			else if (col == 5)
			{
				questData.m_vecFinishOption1.push_back(buffComma);
			}
			else if (col == 6)
			{
				questData.m_vecFinishOption2.push_back(buffComma);
			}
			else if (col == 7)
			{
				questData.m_vecFinishEvent.push_back(buffComma);
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
		if ((col == 7 || col == 8) && (doubleQuoteMode == false))
		{
			row++;
			col = 0;
			m_vecQuestData.push_back(questData);
			questData = QuestData();
		}
	}

	return true;
}
