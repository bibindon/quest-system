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
        
        // テストしたいこと
        // コンストラクタで落ちないこと
        TEST_METHOD(TestMethod01)
        {
            QuestSystem qs;
        }

        // テストしたいこと
        // Init関数で落ちないこと
        TEST_METHOD(TestMethod02)
        {
            QuestSystem qs;
            bool ret = qs.Init("..\\UnitTest1\\sample02.csv");
            Assert::AreEqual(true, ret);
        }

        // テストしたいこと
        // SetTalk関数で落ちないこと
        TEST_METHOD(TestMethod03)
        {
            QuestSystem qs;
            bool ret = qs.Init("..\\UnitTest1\\sample03.csv");
            qs.SetTalk("きんにくん");
            Assert::AreEqual(true, ret);
        }
        
        // テストしたいこと
        // きんにくんに話しかけたらクエストQ1が開始し、Q2は開始しないこと
        TEST_METHOD(TestMethod04)
        {
            QuestSystem qs;
            bool ret = qs.Init("..\\UnitTest1\\sample04.csv");
            qs.SetTalk("きんにくん");

            std::vector<std::string> vs = qs.GetStartQuest();
            auto it = std::find(vs.begin(), vs.end(), "Q1");
            Assert::AreEqual(true, it != vs.end());

            auto it2 = std::find(vs.begin(), vs.end(), "Q2");
            Assert::AreEqual(true, it2 != vs.end());
        }

        // テストしたいこと
        // きんにくんに話しかけ、スライムを３回倒し、再度きんにくんに話しかけたら
        // クエストQ1が完了すること
        TEST_METHOD(TestMethod05)
        {
            QuestSystem qs;
            bool ret = qs.Init("..\\UnitTest1\\sample05.csv");

            qs.SetTalk("きんにくん");
            qs.SetDefeatEnemy("スライム");
            qs.SetDefeatEnemy("スライム");
            qs.SetDefeatEnemy("スライム");
            qs.SetTalk("きんにくん");

            std::vector<std::string> finishQuest = qs.GetFinishQuest();

            Assert::AreEqual((size_t)1,
                             finishQuest.size());

            Assert::AreEqual("Q1",
                             finishQuest.at(0).c_str());

            std::vector<std::string> finishEvent = qs.GetQuestFinishEvent("Q1");

            Assert::AreEqual("<speak><きんにくん>ありがとうございました",
                             finishEvent.at(0).c_str());
        }

        // テストしたいこと
        // きんにくんに話しかけ、スライムを２回しか倒さず、再度きんにくんに話しかけても
        // クエストQ1が完了しないこと
        TEST_METHOD(TestMethod06)
        {
            QuestSystem qs;
            bool ret = qs.Init("..\\UnitTest1\\sample06.csv");

            qs.SetTalk("きんにくん");
            qs.SetDefeatEnemy("スライム");
            qs.SetDefeatEnemy("スライム");
            qs.SetTalk("きんにくん");

            std::vector<std::string> finishQuest = qs.GetFinishQuest();

            Assert::AreEqual((size_t)0,
                             finishQuest.size());
        }

        // テストしたいこと
        // 座標移動により、クエストが開始すること
        TEST_METHOD(TestMethod07)
        {
            QuestSystem qs;
            bool ret = qs.Init("..\\UnitTest1\\sample07.csv");

            std::vector<std::string> startedQuest;
            startedQuest = qs.GetStartQuest();

            Assert::AreEqual(0, (int)startedQuest.size());

            qs.SetPos(0.f, 0.f, 0.f);

            startedQuest = qs.GetStartQuest();

            Assert::AreEqual(1, (int)startedQuest.size());
            Assert::AreEqual("Q5", startedQuest.at(0).c_str());
        }

        // テストしたいこと
        // 座標移動により、クエストが開始し、
        // 座標移動により、クエストが完了すること
        TEST_METHOD(TestMethod08)
        {
            QuestSystem qs;
            bool ret = qs.Init("..\\UnitTest1\\sample08.csv");

            qs.SetPos(0.f, 0.f, 0.f);

            std::vector<std::string> startedQuest = qs.GetStartQuest();

            Assert::AreEqual(1, (int)startedQuest.size());
            Assert::AreEqual("Q5", startedQuest.at(0).c_str());

            qs.SetPos(30.f, 0.f, 0.f);

            std::vector<std::string> finishQuest = qs.GetFinishQuest();

            Assert::AreEqual(1, (int)finishQuest.size());
            Assert::AreEqual("Q5", finishQuest.at(0).c_str());
        }

        // テストしたいこと
        // クエスト開始位置と少し離れていても3メートル以内だったらクエストが開始するか
        TEST_METHOD(TestMethod09)
        {
            QuestSystem qs;
            bool ret = qs.Init("..\\UnitTest1\\sample09.csv");

            qs.SetPos(5.f, 0.f, 5.f);

            std::vector<std::string> startedQuest = qs.GetStartQuest();
            Assert::AreEqual(std::size_t(1), startedQuest.size());
            Assert::AreEqual(std::string("Q5"), startedQuest.at(0));

            qs.SetPos(25.f, 5.f, 0.f);

            std::vector<std::string> finishQuest = qs.GetFinishQuest();
            Assert::AreEqual(std::size_t(1), finishQuest.size());
            Assert::AreEqual(std::string("Q5"), finishQuest.at(0));
        }

        // テストしたいこと
        // クエスト開始位置を二度訪れたとき、クエスト開始済みのフラグが解除されないことのテスト
        TEST_METHOD(TestMethod10)
        {
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

        // テストしたいこと
        // クエスト完了後、再度クエスト開始位置を訪れたときにクエストが開始しないこと
        TEST_METHOD(TestMethod11)
        {
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

        // テストしたいこと
        // クエスト完了後、再度クエスト完了位置を訪れたときにクエストが完了しないこと
        TEST_METHOD(TestMethod12)
        {
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

        // テストしたいこと
        TEST_METHOD(TestMethod13)
        {
            QuestSystem qs;
            bool ret = qs.Init("..\\UnitTest1\\sample13.csv");
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

        // テストしたいこと
        TEST_METHOD(TestMethod14)
        {
            QuestSystem qs;
            bool ret = qs.Init("..\\UnitTest1\\sample14.csv");
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

        // テストしたいこと
        TEST_METHOD(TestMethod15)
        {
            QuestSystem qs;
            bool ret = qs.Init("..\\UnitTest1\\sample15.csv");
            qs.SetExamine(0.f, 0.f, 0.f);
            std::vector<std::string> vs = qs.GetStartQuest();
            std::vector<std::string>::iterator it = std::find(vs.begin(), vs.end(), "Q9");
            Assert::AreEqual(it != vs.end(), true);
            qs.SetTalk("三郎");
            vs = qs.GetFinishQuest();
            it = std::find(vs.begin(), vs.end(), "Q9");
            Assert::AreEqual(it != vs.end(), true);
        }

        // テストしたいこと
        TEST_METHOD(TestMethod16)
        {
            QuestSystem qs;
            bool ret = qs.Init("..\\UnitTest1\\sample16.csv");
            qs.SetTalk("四郎");
            std::vector<std::string> vs = qs.GetStartQuest();
            std::vector<std::string>::iterator it = std::find(vs.begin(), vs.end(), "Q10");
            Assert::AreEqual(it != vs.end(), true);
            qs.SetExamine(0.f, 0.f, 0.f);
            vs = qs.GetFinishQuest();
            it = std::find(vs.begin(), vs.end(), "Q10");
            Assert::AreEqual(it != vs.end(), true);
        }

        // テストしたいこと
        TEST_METHOD(TestMethod17)
        {
            // 通常ケース
            {
                QuestSystem qs;
                bool ret = qs.Init("..\\UnitTest1\\sample17.csv");
                std::string result = qs.GetQuestIdStartByExamine(0.f, 0.f, 0.f);
                Assert::AreEqual(result.c_str(), "Q9");
            }
            // 取得できるのはまだ開始していないクエストだけ。
            // 開始済みだったり、完了済みのクエストは取得できない。
            {
                QuestSystem qs;
                bool ret = qs.Init("..\\UnitTest1\\sample17.csv");
                qs.SetExamine(0.f, 0.f, 0.f);
                std::string result = qs.GetQuestIdStartByExamine(0.f, 0.f, 0.f);
                Assert::AreEqual((int)result.size(), 0);
            }
        }

        // テストしたいこと
        TEST_METHOD(TestMethod18)
        {
            // 通常ケース
            {
                QuestSystem qs;
                bool ret = qs.Init("..\\UnitTest1\\sample18.csv");
                qs.SetTalk("四郎");
                std::string result = qs.GetQuestIdFinishByExamine(0.f, 0.f, 0.f);
                Assert::AreEqual(result.c_str(), "Q10");
            }
            // 取得できるのは開始済みで完了していないクエストだけ。
            // 開始していなかったり、完了済みのクエストは取得できない。
            {
                QuestSystem qs;
                bool ret = qs.Init("..\\UnitTest1\\sample18.csv");
                std::string result = qs.GetQuestIdFinishByExamine(0.f, 0.f, 0.f);
                Assert::AreEqual((int)result.size(), 0);
            }
        }
    };
}
