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
            qs.SetTalk("きんにくん");
            Assert::AreEqual(ret, true);
        }
        TEST_METHOD(TestMethod4)
        {
            QuestSystem qs;
            bool ret = qs.Init("sample.csv");
            qs.SetTalk("きんにくん");
            std::vector<std::string> vs = qs.GetStartQuest();
            auto it = std::find(vs.begin(), vs.end(), "Q1");
            Assert::AreEqual(it != vs.end(), true);
            auto it2 = std::find(vs.begin(), vs.end(), "Q2");
            Assert::AreEqual(it2 != vs.end(), true);
        }
        TEST_METHOD(TestMethod5)
        {
            QuestSystem qs;
            bool ret = qs.Init("sample.csv");
            qs.SetTalk("きんにくん");
            qs.SetDefeatEnemy("スライム");
            qs.SetDefeatEnemy("スライム");
            qs.SetDefeatEnemy("スライム");
            qs.SetTalk("きんにくん");
            std::vector<std::string> finishQuest = qs.GetFinishQuest();
            Assert::AreEqual(finishQuest.size(), (size_t)1);
            Assert::AreEqual(finishQuest.at(0).c_str(), "Q1");
            std::vector<std::string> finishEvent = qs.GetQuestFinishEvent("Q1");
            Assert::AreEqual(finishEvent.at(0).c_str(), "<speak><きんにくん>ありがとうございました");
        }
        TEST_METHOD(TestMethod6)
        {
            QuestSystem qs;
            bool ret = qs.Init("sample.csv");
            qs.SetTalk("きんにくん");
            qs.SetDefeatEnemy("スライム");
            qs.SetDefeatEnemy("スライム");
            qs.SetTalk("きんにくん");
            std::vector<std::string> finishQuest = qs.GetFinishQuest();
            Assert::AreEqual(finishQuest.size(), (size_t)0);
        }
        TEST_METHOD(TestMethod7)
        {
            QuestSystem qs;
            bool ret = qs.Init("sample.csv");
            qs.SetTalk("きんにくん");
            std::vector<std::string> startedQuest = qs.GetStartQuest();
            std::cout << startedQuest.at(0) << std::endl; // "Q1"
            qs.SetDefeatEnemy("スライム");
            qs.SetDefeatEnemy("スライム");
            qs.SetDefeatEnemy("スライム");
            qs.SetTalk("きんにくん");
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
            // クエスト開始位置と少しはなれば場所でもクエストが開始するかどうかのテスト
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
            // クエスト開始位置を二度訪れたとき、クエスト開始済みのフラグが解除されないことのテスト
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
            // クエスト完了後、再度クエスト開始位置を訪れたときにクエストが開始しないことのテスト
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
        TEST_METHOD(TestMethod12)
        {
            // クエスト完了後、再度クエスト完了位置を訪れたときにクエストが完了しないことのテスト
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
            qs.SetPos(25.f, 5.f, 0.f);
            startedQuest = qs.GetStartQuest();
            Assert::AreEqual(startedQuest.size(), (size_t)0);
        }
        TEST_METHOD(TestMethod13)
        {
            QuestSystem qs;
            bool ret = qs.Init("sample.csv");
            qs.SetTalk("きんにくん");
            std::vector<std::string> vs = qs.GetStartQuest();
            qs.SetTalk("シュワちゃん");
            vs = qs.GetFinishQuest();

            std::vector<std::string>::iterator it = std::find(vs.begin(), vs.end(), "Q6");
            Assert::AreEqual(it != vs.end(), true);

            std::vector<std::string> startedQuest = qs.GetStartQuest();
            Assert::AreEqual(startedQuest.size(), (size_t)1);
            Assert::AreEqual(startedQuest.at(0).c_str(), "Q7");
            qs.SetTalk("太郎");
            vs= qs.GetFinishQuest();
            Assert::AreEqual(vs.size(), (size_t)1);
            Assert::AreEqual(vs.at(0).c_str(), "Q7");
        }
        TEST_METHOD(TestMethod14)
        {
            QuestSystem qs;
            bool ret = qs.Init("sample.csv");
            qs.SetTalk("次郎");
            std::vector<std::string> vs = qs.GetStartQuest();
            std::vector<std::string>::iterator it = std::find(vs.begin(), vs.end(), "Q8");
            Assert::AreEqual(it != vs.end(), true);
            qs.SetTalk("きんにくん");
            qs.SetTalk("シュワちゃん");
            vs = qs.GetFinishQuest();
            it = std::find(vs.begin(), vs.end(), "Q8");
            Assert::AreEqual(it != vs.end(), true);
        }
        TEST_METHOD(TestMethod15)
        {
            QuestSystem qs;
            bool ret = qs.Init("sample.csv");
            qs.SetExamine(0.f, 0.f, 0.f);
            std::vector<std::string> vs = qs.GetStartQuest();
            std::vector<std::string>::iterator it = std::find(vs.begin(), vs.end(), "Q9");
            Assert::AreEqual(it != vs.end(), true);
            qs.SetTalk("三郎");
            vs = qs.GetFinishQuest();
            it = std::find(vs.begin(), vs.end(), "Q9");
            Assert::AreEqual(it != vs.end(), true);
        }
        TEST_METHOD(TestMethod16)
        {
            QuestSystem qs;
            bool ret = qs.Init("sample.csv");
            qs.SetTalk("四郎");
            std::vector<std::string> vs = qs.GetStartQuest();
            std::vector<std::string>::iterator it = std::find(vs.begin(), vs.end(), "Q10");
            Assert::AreEqual(it != vs.end(), true);
            qs.SetExamine(0.f, 0.f, 0.f);
            vs = qs.GetFinishQuest();
            it = std::find(vs.begin(), vs.end(), "Q10");
            Assert::AreEqual(it != vs.end(), true);
        }
        TEST_METHOD(TestMethod17)
        {
            // 通常ケース
            {
                QuestSystem qs;
                bool ret = qs.Init("sample.csv");
                std::string result = qs.GetQuestIdStartByExamine(0.f, 0.f, 0.f);
                Assert::AreEqual(result.c_str(), "Q9");
            }
            // 取得できるのはまだ開始していないクエストだけ。
            // 開始済みだったり、完了済みのクエストは取得できない。
            {
                QuestSystem qs;
                bool ret = qs.Init("sample.csv");
                qs.SetExamine(0.f, 0.f, 0.f);
                std::string result = qs.GetQuestIdStartByExamine(0.f, 0.f, 0.f);
                Assert::AreEqual((int)result.size(), 0);
            }
        }
        TEST_METHOD(TestMethod18)
        {
            // 通常ケース
            {
                QuestSystem qs;
                bool ret = qs.Init("sample.csv");
                qs.SetTalk("四郎");
                std::string result = qs.GetQuestIdFinishByExamine(0.f, 0.f, 0.f);
                Assert::AreEqual(result.c_str(), "Q10");
            }
            // 取得できるのは開始済みで完了していないクエストだけ。
            // 開始していなかったり、完了済みのクエストは取得できない。
            {
                QuestSystem qs;
                bool ret = qs.Init("sample.csv");
                std::string result = qs.GetQuestIdFinishByExamine(0.f, 0.f, 0.f);
                Assert::AreEqual((int)result.size(), 0);
            }
        }
    };
}
