#include "pch.h"
#include "CppUnitTest.h"
#include "../QuestSystem/QuestSystem.h"
#include <iostream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace NSQuestSystem;

namespace UnitTest1
{
    TEST_CLASS(UnitTest1)
    {
    public:
        
        // �e�X�g����������
        // �R���X�g���N�^�ŗ����Ȃ�����
        TEST_METHOD(TestMethod01)
        {
            QuestSystem qs;
        }

        // �e�X�g����������
        // Init�֐��ŗ����Ȃ�����
        TEST_METHOD(TestMethod02)
        {
            QuestSystem qs;
            bool ret = qs.Init("..\\UnitTest1\\sample02.csv");
            Assert::AreEqual(true, ret);
        }

        // �e�X�g����������
        // SetTalk�֐��ŗ����Ȃ�����
        TEST_METHOD(TestMethod03)
        {
            QuestSystem qs;
            bool ret = qs.Init("..\\UnitTest1\\sample03.csv");
            qs.SetTalk("����ɂ���");
            Assert::AreEqual(true, ret);
        }
        
        // �e�X�g����������
        // ����ɂ���ɘb����������N�G�X�gQ1���J�n���AQ2�͊J�n���Ȃ�����
        TEST_METHOD(TestMethod04)
        {
            QuestSystem qs;
            bool ret = qs.Init("..\\UnitTest1\\sample04.csv");
            qs.SetTalk("����ɂ���");

            std::vector<std::string> vs = qs.GetStartQuest();
            auto it = std::find(vs.begin(), vs.end(), "Q1");
            Assert::AreEqual(true, it != vs.end());

            auto it2 = std::find(vs.begin(), vs.end(), "Q2");
            Assert::AreEqual(true, it2 != vs.end());
        }

        // �e�X�g����������
        TEST_METHOD(TestMethod05)
        {
            QuestSystem qs;
            bool ret = qs.Init("..\\UnitTest1\\sample05.csv");
            qs.SetTalk("����ɂ���");
            qs.SetDefeatEnemy("�X���C��");
            qs.SetDefeatEnemy("�X���C��");
            qs.SetDefeatEnemy("�X���C��");
            qs.SetTalk("����ɂ���");
            std::vector<std::string> finishQuest = qs.GetFinishQuest();

            Assert::AreEqual((size_t)1, finishQuest.size());
            Assert::AreEqual("Q1", finishQuest.at(0).c_str());

            std::vector<std::string> finishEvent = qs.GetQuestFinishEvent("Q1");

            Assert::AreEqual(finishEvent.at(0).c_str(), "<speak><����ɂ���>���肪�Ƃ��������܂���");
        }

        // �e�X�g����������
        TEST_METHOD(TestMethod06)
        {
            QuestSystem qs;
            bool ret = qs.Init("..\\UnitTest1\\sample06.csv");
            qs.SetTalk("����ɂ���");
            qs.SetDefeatEnemy("�X���C��");
            qs.SetDefeatEnemy("�X���C��");
            qs.SetTalk("����ɂ���");
            std::vector<std::string> finishQuest = qs.GetFinishQuest();
            Assert::AreEqual(finishQuest.size(), (size_t)0);
        }

        // �e�X�g����������
        TEST_METHOD(TestMethod07)
        {
            QuestSystem qs;
            bool ret = qs.Init("..\\UnitTest1\\sample07.csv");
            qs.SetTalk("����ɂ���");
            std::vector<std::string> startedQuest = qs.GetStartQuest();
            std::cout << startedQuest.at(0) << std::endl; // "Q1"
            qs.SetDefeatEnemy("�X���C��");
            qs.SetDefeatEnemy("�X���C��");
            qs.SetDefeatEnemy("�X���C��");
            qs.SetTalk("����ɂ���");
            std::vector<std::string> finishQuest = qs.GetFinishQuest();
            Assert::AreEqual(finishQuest.size(), (size_t)1);
            Assert::AreEqual(finishQuest.at(0).c_str(), "Q1");
        }

        // �e�X�g����������
        TEST_METHOD(TestMethod08)
        {
            QuestSystem qs;
            bool ret = qs.Init("..\\UnitTest1\\sample08.csv");
            qs.SetPos(0.f, 0.f, 0.f);
            std::vector<std::string> startedQuest = qs.GetStartQuest();
            Assert::AreEqual(startedQuest.size(), (size_t)1);
            Assert::AreEqual(startedQuest.at(0).c_str(), "Q5");
            qs.SetPos(30.f, 0.f, 0.f);
            std::vector<std::string> finishQuest = qs.GetFinishQuest();
            Assert::AreEqual(finishQuest.size(), (size_t)1);
            Assert::AreEqual(finishQuest.at(0).c_str(), "Q5");
        }

        // �e�X�g����������
        TEST_METHOD(TestMethod09)
        {
            // �N�G�X�g�J�n�ʒu�Ə����͂Ȃ�Ώꏊ�ł��N�G�X�g���J�n���邩�ǂ����̃e�X�g
            QuestSystem qs;
            bool ret = qs.Init("..\\UnitTest1\\sample09.csv");
            qs.SetPos(5.f, 0.f, 5.f);
            std::vector<std::string> startedQuest = qs.GetStartQuest();
            Assert::AreEqual(startedQuest.size(), (size_t)1);
            Assert::AreEqual(startedQuest.at(0).c_str(), "Q5");
            qs.SetPos(25.f, 5.f, 0.f);
            std::vector<std::string> finishQuest = qs.GetFinishQuest();
            Assert::AreEqual(finishQuest.size(), (size_t)1);
            Assert::AreEqual(finishQuest.at(0).c_str(), "Q5");
        }

        // �e�X�g����������
        TEST_METHOD(TestMethod10)
        {
            // �N�G�X�g�J�n�ʒu���x�K�ꂽ�Ƃ��A�N�G�X�g�J�n�ς݂̃t���O����������Ȃ����Ƃ̃e�X�g
            QuestSystem qs;
            bool ret = qs.Init("..\\UnitTest1\\sample10.csv");
            qs.SetPos(5.f, 0.f, 5.f);
            qs.SetPos(5.f, 0.f, 5.f);
            std::vector<std::string> startedQuest = qs.GetStartQuest();
            Assert::AreEqual(startedQuest.size(), (size_t)1);
            Assert::AreEqual(startedQuest.at(0).c_str(), "Q5");
            qs.SetPos(25.f, 5.f, 0.f);
            qs.SetPos(25.f, 5.f, 0.f);
            std::vector<std::string> finishQuest = qs.GetFinishQuest();
            Assert::AreEqual(finishQuest.size(), (size_t)1);
            Assert::AreEqual(finishQuest.at(0).c_str(), "Q5");
        }

        // �e�X�g����������
        TEST_METHOD(TestMethod11)
        {
            // �N�G�X�g������A�ēx�N�G�X�g�J�n�ʒu��K�ꂽ�Ƃ��ɃN�G�X�g���J�n���Ȃ����Ƃ̃e�X�g
            QuestSystem qs;
            bool ret = qs.Init("..\\UnitTest1\\sample11.csv");
            qs.SetPos(5.f, 0.f, 5.f);
            std::vector<std::string> startedQuest = qs.GetStartQuest();
            Assert::AreEqual(startedQuest.size(), (size_t)1);
            Assert::AreEqual(startedQuest.at(0).c_str(), "Q5");
            qs.SetPos(25.f, 5.f, 0.f);
            std::vector<std::string> finishQuest = qs.GetFinishQuest();
            Assert::AreEqual(finishQuest.size(), (size_t)1);
            Assert::AreEqual(finishQuest.at(0).c_str(), "Q5");
            qs.SetPos(5.f, 0.f, 5.f);
            startedQuest = qs.GetStartQuest();
            Assert::AreEqual(startedQuest.size(), (size_t)0);
        }

        // �e�X�g����������
        TEST_METHOD(TestMethod12)
        {
            // �N�G�X�g������A�ēx�N�G�X�g�����ʒu��K�ꂽ�Ƃ��ɃN�G�X�g���������Ȃ����Ƃ̃e�X�g
            QuestSystem qs;
            bool ret = qs.Init("..\\UnitTest1\\sample12.csv");
            qs.SetPos(5.f, 0.f, 5.f);
            std::vector<std::string> startedQuest = qs.GetStartQuest();
            Assert::AreEqual(startedQuest.size(), (size_t)1);
            Assert::AreEqual(startedQuest.at(0).c_str(), "Q5");
            qs.SetPos(25.f, 5.f, 0.f);
            std::vector<std::string> finishQuest = qs.GetFinishQuest();
            Assert::AreEqual(finishQuest.size(), (size_t)1);
            Assert::AreEqual(finishQuest.at(0).c_str(), "Q5");
            qs.SetPos(25.f, 5.f, 0.f);
            startedQuest = qs.GetStartQuest();
            Assert::AreEqual(startedQuest.size(), (size_t)0);
        }

        // �e�X�g����������
        TEST_METHOD(TestMethod13)
        {
            QuestSystem qs;
            bool ret = qs.Init("..\\UnitTest1\\sample13.csv");
            qs.SetTalk("����ɂ���");
            std::vector<std::string> vs = qs.GetStartQuest();
            qs.SetTalk("�V���������");
            vs = qs.GetFinishQuest();

            std::vector<std::string>::iterator it = std::find(vs.begin(), vs.end(), "Q6");
            Assert::AreEqual(it != vs.end(), true);

            std::vector<std::string> startedQuest = qs.GetStartQuest();
            Assert::AreEqual(startedQuest.size(), (size_t)1);
            Assert::AreEqual(startedQuest.at(0).c_str(), "Q7");
            qs.SetTalk("���Y");
            vs= qs.GetFinishQuest();
            Assert::AreEqual(vs.size(), (size_t)1);
            Assert::AreEqual(vs.at(0).c_str(), "Q7");
        }

        // �e�X�g����������
        TEST_METHOD(TestMethod14)
        {
            QuestSystem qs;
            bool ret = qs.Init("..\\UnitTest1\\sample14.csv");
            qs.SetTalk("���Y");
            std::vector<std::string> vs = qs.GetStartQuest();
            std::vector<std::string>::iterator it = std::find(vs.begin(), vs.end(), "Q8");
            Assert::AreEqual(it != vs.end(), true);
            qs.SetTalk("����ɂ���");
            qs.SetTalk("�V���������");
            vs = qs.GetFinishQuest();
            it = std::find(vs.begin(), vs.end(), "Q8");
            Assert::AreEqual(it != vs.end(), true);
        }

        // �e�X�g����������
        TEST_METHOD(TestMethod15)
        {
            QuestSystem qs;
            bool ret = qs.Init("..\\UnitTest1\\sample15.csv");
            qs.SetExamine(0.f, 0.f, 0.f);
            std::vector<std::string> vs = qs.GetStartQuest();
            std::vector<std::string>::iterator it = std::find(vs.begin(), vs.end(), "Q9");
            Assert::AreEqual(it != vs.end(), true);
            qs.SetTalk("�O�Y");
            vs = qs.GetFinishQuest();
            it = std::find(vs.begin(), vs.end(), "Q9");
            Assert::AreEqual(it != vs.end(), true);
        }

        // �e�X�g����������
        TEST_METHOD(TestMethod16)
        {
            QuestSystem qs;
            bool ret = qs.Init("..\\UnitTest1\\sample16.csv");
            qs.SetTalk("�l�Y");
            std::vector<std::string> vs = qs.GetStartQuest();
            std::vector<std::string>::iterator it = std::find(vs.begin(), vs.end(), "Q10");
            Assert::AreEqual(it != vs.end(), true);
            qs.SetExamine(0.f, 0.f, 0.f);
            vs = qs.GetFinishQuest();
            it = std::find(vs.begin(), vs.end(), "Q10");
            Assert::AreEqual(it != vs.end(), true);
        }

        // �e�X�g����������
        TEST_METHOD(TestMethod17)
        {
            // �ʏ�P�[�X
            {
                QuestSystem qs;
                bool ret = qs.Init("..\\UnitTest1\\sample17.csv");
                std::string result = qs.GetQuestIdStartByExamine(0.f, 0.f, 0.f);
                Assert::AreEqual(result.c_str(), "Q9");
            }
            // �擾�ł���̂͂܂��J�n���Ă��Ȃ��N�G�X�g�����B
            // �J�n�ς݂�������A�����ς݂̃N�G�X�g�͎擾�ł��Ȃ��B
            {
                QuestSystem qs;
                bool ret = qs.Init("..\\UnitTest1\\sample17.csv");
                qs.SetExamine(0.f, 0.f, 0.f);
                std::string result = qs.GetQuestIdStartByExamine(0.f, 0.f, 0.f);
                Assert::AreEqual((int)result.size(), 0);
            }
        }

        // �e�X�g����������
        TEST_METHOD(TestMethod18)
        {
            // �ʏ�P�[�X
            {
                QuestSystem qs;
                bool ret = qs.Init("..\\UnitTest1\\sample18.csv");
                qs.SetTalk("�l�Y");
                std::string result = qs.GetQuestIdFinishByExamine(0.f, 0.f, 0.f);
                Assert::AreEqual(result.c_str(), "Q10");
            }
            // �擾�ł���̂͊J�n�ς݂Ŋ������Ă��Ȃ��N�G�X�g�����B
            // �J�n���Ă��Ȃ�������A�����ς݂̃N�G�X�g�͎擾�ł��Ȃ��B
            {
                QuestSystem qs;
                bool ret = qs.Init("..\\UnitTest1\\sample18.csv");
                std::string result = qs.GetQuestIdFinishByExamine(0.f, 0.f, 0.f);
                Assert::AreEqual((int)result.size(), 0);
            }
        }
    };
}
