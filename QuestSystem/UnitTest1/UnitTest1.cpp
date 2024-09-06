#include "pch.h"
#include "CppUnitTest.h"
#include "../QuestSystem/QuestSystem.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

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
            // TODO 3�C�|������A�Ƃ����Ƃ��낪�ł��Ă��Ȃ��B
            qs.SetDefeatEnemy("�X���C��");
            qs.SetDefeatEnemy("�X���C��");
            qs.SetTalk("����ɂ���");
            std::vector<std::string> finishQuest = qs.GetFinishQuest();
            Assert::AreEqual(finishQuest.size(), (size_t)0);
        }
    };
}
