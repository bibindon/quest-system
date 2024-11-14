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
        
        TEST_METHOD(TestMethod1)
        {
            QuestSystem qs;
        }
        TEST_METHOD(TestMethod2)
        {
            QuestSystem qs;
            bool ret = qs.Init("sample.csv");
            Assert::AreEqual(ret, true);
        }
        TEST_METHOD(TestMethod3)
        {
            QuestSystem qs;
            bool ret = qs.Init("sample.csv");
            qs.SetTalk("����ɂ���");
            Assert::AreEqual(ret, true);
        }
        TEST_METHOD(TestMethod4)
        {
            QuestSystem qs;
            bool ret = qs.Init("sample.csv");
            qs.SetTalk("����ɂ���");
            std::vector<std::string> startedQuest = qs.GetStartQuest();
            Assert::AreEqual(startedQuest.size(), (size_t)2);
            Assert::AreEqual(startedQuest.at(0).c_str(), "Q1");
            Assert::AreEqual(startedQuest.at(1).c_str(), "Q2");
        }
        TEST_METHOD(TestMethod5)
        {
            QuestSystem qs;
            bool ret = qs.Init("sample.csv");
            qs.SetTalk("����ɂ���");
            qs.SetDefeatEnemy("�X���C��");
            qs.SetDefeatEnemy("�X���C��");
            qs.SetDefeatEnemy("�X���C��");
            qs.SetTalk("����ɂ���");
            std::vector<std::string> finishQuest = qs.GetFinishQuest();
            Assert::AreEqual(finishQuest.size(), (size_t)1);
            Assert::AreEqual(finishQuest.at(0).c_str(), "Q1");
        }
        TEST_METHOD(TestMethod6)
        {
            QuestSystem qs;
            bool ret = qs.Init("sample.csv");
            qs.SetTalk("����ɂ���");
            qs.SetDefeatEnemy("�X���C��");
            qs.SetDefeatEnemy("�X���C��");
            qs.SetTalk("����ɂ���");
            std::vector<std::string> finishQuest = qs.GetFinishQuest();
            Assert::AreEqual(finishQuest.size(), (size_t)0);
        }
        TEST_METHOD(TestMethod7)
        {
            QuestSystem qs;
            bool ret = qs.Init("sample.csv");
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
        TEST_METHOD(TestMethod8)
        {
            QuestSystem qs;
            bool ret = qs.Init("sample.csv");
            qs.SetPos(0.f, 0.f, 0.f);
            std::vector<std::string> startedQuest = qs.GetStartQuest();
            Assert::AreEqual(startedQuest.size(), (size_t)1);
            Assert::AreEqual(startedQuest.at(0).c_str(), "Q5");
            qs.SetPos(30.f, 0.f, 0.f);
            std::vector<std::string> finishQuest = qs.GetFinishQuest();
            Assert::AreEqual(finishQuest.size(), (size_t)1);
            Assert::AreEqual(finishQuest.at(0).c_str(), "Q5");
        }
        TEST_METHOD(TestMethod9)
        {
            // �N�G�X�g�J�n�ʒu�Ə����͂Ȃ�Ώꏊ�ł��N�G�X�g���J�n���邩�ǂ����̃e�X�g
            QuestSystem qs;
            bool ret = qs.Init("sample.csv");
            qs.SetPos(5.f, 0.f, 5.f);
            std::vector<std::string> startedQuest = qs.GetStartQuest();
            Assert::AreEqual(startedQuest.size(), (size_t)1);
            Assert::AreEqual(startedQuest.at(0).c_str(), "Q5");
            qs.SetPos(25.f, 5.f, 0.f);
            std::vector<std::string> finishQuest = qs.GetFinishQuest();
            Assert::AreEqual(finishQuest.size(), (size_t)1);
            Assert::AreEqual(finishQuest.at(0).c_str(), "Q5");
        }
        TEST_METHOD(TestMethod10)
        {
            // �N�G�X�g�J�n�ʒu���x�K�ꂽ�Ƃ��A�N�G�X�g�J�n�ς݂̃t���O����������Ȃ����Ƃ̃e�X�g
            QuestSystem qs;
            bool ret = qs.Init("sample.csv");
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
        TEST_METHOD(TestMethod11)
        {
            // �N�G�X�g������A�ēx�N�G�X�g�J�n�ʒu��K�ꂽ�Ƃ��ɃN�G�X�g���J�n���Ȃ����Ƃ̃e�X�g
            QuestSystem qs;
            bool ret = qs.Init("sample.csv");
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
    };
}
