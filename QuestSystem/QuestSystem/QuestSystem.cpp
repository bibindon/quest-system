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
				questData.m_id = buffComma;
			}
			else if (col == 1)
			{
				if (buffComma == "�l�Ƙb������")
				{
					questData.m_vecStartType.push_back(eStartType::TALK);
				}
				else if (buffComma == "�N�G�X�g���������Ă�����")
				{
					questData.m_vecStartType.push_back(eStartType::QUEST_FINISHED);
				}
				else if (buffComma == "�N�G�X�g���������Ă��Ȃ��Ȃ�")
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
				if (buffComma == "�G��|������")
				{
					questData.m_vecFinishType.push_back(eFinishType::DEFEAT_ENEMY);
				}
				else if (buffComma == "�l�Ƙb������")
				{
					questData.m_vecFinishType.push_back(eFinishType::TALK);
				}
				else if (buffComma == "��������")
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
		// �ʏ�̃N�G�X�g�f�[�^�͏�L�̂悤�ȃf�[�^�ł��邪
		// 0,1,2,3,4,5,6,
		// ��L�̂悤�ɍŌ�̗�ɉ����Ȃ���������B
		// ���̂Ƃ�getline�֐���'6'�܂ł����ǂ߂Ȃ��B
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
