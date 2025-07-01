#include "pch.h"
#include "CppUnitTest.h"
#include "../QuestSystem/QuestSystem.h"
#include <iostream>
#include <tchar.h>

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
            bool ret = qs.Init(_T("..\\UnitTest1\\sample02.csv"), _T(""), false);
            Assert::AreEqual(true, ret);
        }

        // テストしたいこと
        // SetTalk関数で落ちないこと
        TEST_METHOD(TestMethod03)
        {
            QuestSystem qs;
            bool ret = qs.Init(_T("..\\UnitTest1\\sample03.csv"), _T(""), false);
            qs.SetTalk(_T("きんにくん"));
            Assert::AreEqual(true, ret);
        }
        
        // テストしたいこと
        // きんにくんに話しかけたらクエストQ1が開始し、Q2は開始しないこと
        TEST_METHOD(TestMethod04)
        {
            QuestSystem qs;
            bool ret = qs.Init(_T("..\\UnitTest1\\sample04.csv"), _T(""), false);
            qs.SetTalk(_T("きんにくん"));

            std::vector<std::wstring> vs = qs.GetStartQuest();
            auto it = std::find(vs.begin(), vs.end(), _T("Q1"));
            Assert::AreEqual(true, it != vs.end());

            auto it2 = std::find(vs.begin(), vs.end(), _T("Q2"));
            Assert::AreEqual(true, it2 != vs.end());
        }

        // テストしたいこと
        // きんにくんに話しかけ、スライムを３回倒し、再度きんにくんに話しかけたら
        // クエストQ1が完了すること
        TEST_METHOD(TestMethod05)
        {
            QuestSystem qs;
            bool ret = qs.Init(_T("..\\UnitTest1\\sample05.csv"), _T(""), false);

            qs.SetTalk(_T("きんにくん"));
            qs.SetDefeatEnemy(_T("スライム"));
            qs.SetDefeatEnemy(_T("スライム"));
            qs.SetDefeatEnemy(_T("スライム"));
            qs.SetTalk(_T("きんにくん"));

            std::vector<std::wstring> finishQuest = qs.GetFinishQuest();

            Assert::AreEqual((size_t)1,
                             finishQuest.size());

            Assert::AreEqual(_T("Q1"),
                             finishQuest.at(0).c_str());

            std::vector<std::wstring> finishEvent = qs.GetQuestFinishEvent(_T("Q1"));

            Assert::AreEqual(_T("<speak><きんにくん>ありがとうございました"),
                             finishEvent.at(0).c_str());
        }

        // テストしたいこと
        // きんにくんに話しかけ、スライムを２回しか倒さず、再度きんにくんに話しかけても
        // クエストQ1が完了しないこと
        TEST_METHOD(TestMethod06)
        {
            QuestSystem qs;
            bool ret = qs.Init(_T("..\\UnitTest1\\sample06.csv"), _T(""), false);

            qs.SetTalk(_T("きんにくん"));
            qs.SetDefeatEnemy(_T("スライム"));
            qs.SetDefeatEnemy(_T("スライム"));
            qs.SetTalk(_T("きんにくん"));

            std::vector<std::wstring> finishQuest = qs.GetFinishQuest();

            Assert::AreEqual((size_t)0,
                             finishQuest.size());
        }

        // テストしたいこと
        // 座標移動により、クエストが開始すること
        TEST_METHOD(TestMethod07)
        {
            QuestSystem qs;
            bool ret = qs.Init(_T("..\\UnitTest1\\sample07.csv"), _T(""), false);

            std::vector<std::wstring> startedQuest;
            startedQuest = qs.GetStartQuest();

            Assert::AreEqual(0, (int)startedQuest.size());

            qs.SetPos(0.f, 0.f, 0.f);

            startedQuest = qs.GetStartQuest();

            Assert::AreEqual(1, (int)startedQuest.size());
            Assert::AreEqual(_T("Q5"), startedQuest.at(0).c_str());
        }

        // テストしたいこと
        // 座標移動により、クエストが開始し、
        // 座標移動により、クエストが完了すること
        TEST_METHOD(TestMethod08)
        {
            QuestSystem qs;
            bool ret = qs.Init(_T("..\\UnitTest1\\sample08.csv"), _T(""), false);

            qs.SetPos(0.f, 0.f, 0.f);

            std::vector<std::wstring> startedQuest = qs.GetStartQuest();

            Assert::AreEqual(1, (int)startedQuest.size());
            Assert::AreEqual(_T("Q5"), startedQuest.at(0).c_str());

            qs.SetPos(30.f, 0.f, 0.f);

            std::vector<std::wstring> finishQuest = qs.GetFinishQuest();

            Assert::AreEqual(1, (int)finishQuest.size());
            Assert::AreEqual(_T("Q5"), finishQuest.at(0).c_str());
        }

        // テストしたいこと
        // クエスト開始位置と少し離れていても3メートル以内だったらクエストが開始するか
        TEST_METHOD(TestMethod09)
        {
            QuestSystem qs;
            bool ret = qs.Init(_T("..\\UnitTest1\\sample09.csv"), _T(""), false);

            qs.SetPos(5.f, 0.f, 5.f);

            std::vector<std::wstring> startedQuest = qs.GetStartQuest();
            Assert::AreEqual(std::size_t(1), startedQuest.size());
            Assert::AreEqual(std::wstring(_T("Q5")), startedQuest.at(0));

            qs.SetPos(25.f, 5.f, 0.f);

            std::vector<std::wstring> finishQuest = qs.GetFinishQuest();
            Assert::AreEqual(std::size_t(1), finishQuest.size());
            Assert::AreEqual(std::wstring(_T("Q5")), finishQuest.at(0));
        }

        // テストしたいこと
        // クエスト開始位置を二度訪れたとき、クエスト開始済みのフラグが解除されないことのテスト
        TEST_METHOD(TestMethod10)
        {
            QuestSystem qs;
            bool ret = qs.Init(_T("..\\UnitTest1\\sample10.csv"), _T(""), false);

            qs.SetPos(5.f, 0.f, 5.f);
            qs.SetPos(5.f, 0.f, 5.f);

            std::vector<std::wstring> startedQuest = qs.GetStartQuest();
            Assert::AreEqual(startedQuest.size(), (size_t)1);
            Assert::AreEqual(startedQuest.at(0).c_str(), _T("Q5"));

            qs.SetPos(25.f, 5.f, 0.f);
            qs.SetPos(25.f, 5.f, 0.f);

            std::vector<std::wstring> finishQuest = qs.GetFinishQuest();
            Assert::AreEqual(finishQuest.size(), (size_t)1);
            Assert::AreEqual(finishQuest.at(0).c_str(), _T("Q5"));
        }

        // テストしたいこと
        // クエスト完了後、再度クエスト開始位置を訪れたときにクエストが開始しないこと
        TEST_METHOD(TestMethod11)
        {
            QuestSystem qs;
            bool ret = qs.Init(_T("..\\UnitTest1\\sample11.csv"), _T(""), false);
            qs.SetPos(5.f, 0.f, 5.f);

            std::vector<std::wstring> startedQuest = qs.GetStartQuest();
            Assert::AreEqual(startedQuest.size(), (size_t)1);
            Assert::AreEqual(startedQuest.at(0).c_str(), _T("Q5"));

            qs.SetPos(25.f, 5.f, 0.f);

            std::vector<std::wstring> finishQuest = qs.GetFinishQuest();
            Assert::AreEqual(finishQuest.size(), (size_t)1);
            Assert::AreEqual(finishQuest.at(0).c_str(), _T("Q5"));

            qs.SetPos(5.f, 0.f, 5.f);

            startedQuest = qs.GetStartQuest();
            Assert::AreEqual(startedQuest.size(), (size_t)0);
        }

        // テストしたいこと
        // クエスト完了後、再度クエスト完了位置を訪れたときにクエストが完了しないこと
        TEST_METHOD(TestMethod12)
        {
            QuestSystem qs;
            bool ret = qs.Init(_T("..\\UnitTest1\\sample12.csv"), _T(""), false);

            qs.SetPos(5.f, 0.f, 5.f);

            std::vector<std::wstring> startedQuest = qs.GetStartQuest();
            Assert::AreEqual(startedQuest.size(), (size_t)1);
            Assert::AreEqual(startedQuest.at(0).c_str(), _T("Q5"));

            qs.SetPos(25.f, 5.f, 0.f);

            std::vector<std::wstring> finishQuest = qs.GetFinishQuest();
            Assert::AreEqual(finishQuest.size(), (size_t)1);
            Assert::AreEqual(finishQuest.at(0).c_str(), _T("Q5"));

            qs.SetPos(25.f, 5.f, 0.f);

            startedQuest = qs.GetStartQuest();
            Assert::AreEqual(startedQuest.size(), (size_t)0);
        }

        // テストしたいこと
        TEST_METHOD(TestMethod13)
        {
            QuestSystem qs;
            bool ret = qs.Init(_T("..\\UnitTest1\\sample13.csv"), _T(""), false);
            qs.SetTalk(_T("きんにくん"));
            std::vector<std::wstring> vs = qs.GetStartQuest();
            qs.SetTalk(_T("シュワちゃん"));
            vs = qs.GetFinishQuest();

            std::vector<std::wstring>::iterator it = std::find(vs.begin(), vs.end(), _T("Q6"));
            Assert::AreEqual(it != vs.end(), true);

            std::vector<std::wstring> startedQuest = qs.GetStartQuest();
            Assert::AreEqual(startedQuest.size(), (size_t)1);
            Assert::AreEqual(startedQuest.at(0).c_str(), _T("Q7"));
            qs.SetTalk(_T("太郎"));
            vs= qs.GetFinishQuest();
            Assert::AreEqual(vs.size(), (size_t)1);
            Assert::AreEqual(vs.at(0).c_str(), _T("Q7"));
        }

        // テストしたいこと
        TEST_METHOD(TestMethod14)
        {
            QuestSystem qs;
            bool ret = qs.Init(_T("..\\UnitTest1\\sample14.csv"), _T(""), false);
            qs.SetTalk(_T("次郎"));
            std::vector<std::wstring> vs = qs.GetStartQuest();
            std::vector<std::wstring>::iterator it = std::find(vs.begin(), vs.end(), _T("Q8"));
            Assert::AreEqual(it != vs.end(), true);
            qs.SetTalk(_T("きんにくん"));
            qs.SetTalk(_T("シュワちゃん"));
            vs = qs.GetFinishQuest();
            it = std::find(vs.begin(), vs.end(), _T("Q8"));
            Assert::AreEqual(it != vs.end(), true);
        }

        // テストしたいこと
        TEST_METHOD(TestMethod15)
        {
            QuestSystem qs;
            bool ret = qs.Init(_T("..\\UnitTest1\\sample15.csv"), _T(""), false);
            qs.SetExamine(0.f, 0.f, 0.f);
            std::vector<std::wstring> vs = qs.GetStartQuest();
            std::vector<std::wstring>::iterator it = std::find(vs.begin(), vs.end(), _T("Q9"));
            Assert::AreEqual(it != vs.end(), true);
            qs.SetTalk(_T("三郎"));
            vs = qs.GetFinishQuest();
            it = std::find(vs.begin(), vs.end(), _T("Q9"));
            Assert::AreEqual(it != vs.end(), true);
        }

        // テストしたいこと
        TEST_METHOD(TestMethod16)
        {
            QuestSystem qs;
            bool ret = qs.Init(_T("..\\UnitTest1\\sample16.csv"), _T(""), false);
            qs.SetTalk(_T("四郎"));
            std::vector<std::wstring> vs = qs.GetStartQuest();
            std::vector<std::wstring>::iterator it = std::find(vs.begin(), vs.end(), _T("Q10"));
            Assert::AreEqual(it != vs.end(), true);
            qs.SetExamine(0.f, 0.f, 0.f);
            vs = qs.GetFinishQuest();
            it = std::find(vs.begin(), vs.end(), _T("Q10"));
            Assert::AreEqual(it != vs.end(), true);
        }

        // テストしたいこと
        TEST_METHOD(TestMethod17)
        {
            // 通常ケース
            {
                QuestSystem qs;
                bool ret = qs.Init(_T("..\\UnitTest1\\sample17.csv"), _T(""), false);
                std::wstring result = qs.GetQuestIdStartByExamine(0.f, 0.f, 0.f);
                Assert::AreEqual(result.c_str(), _T("Q9"));
            }
            // 取得できるのはまだ開始していないクエストだけ。
            // 開始済みだったり、完了済みのクエストは取得できない。
            {
                QuestSystem qs;
                bool ret = qs.Init(_T("..\\UnitTest1\\sample17.csv"), _T(""), false);
                qs.SetExamine(0.f, 0.f, 0.f);
                std::wstring result = qs.GetQuestIdStartByExamine(0.f, 0.f, 0.f);
                Assert::AreEqual((int)result.size(), 0);
            }
        }

        // テストしたいこと
        TEST_METHOD(TestMethod18)
        {
            // 通常ケース
            {
                QuestSystem qs;
                bool ret = qs.Init(_T("..\\UnitTest1\\sample18.csv"), _T(""), false);
                qs.SetTalk(_T("四郎"));
                std::wstring result = qs.GetQuestIdFinishByExamine(0.f, 0.f, 0.f);
                Assert::AreEqual(result.c_str(), _T("Q10"));
            }
            // 取得できるのは開始済みで完了していないクエストだけ。
            // 開始していなかったり、完了済みのクエストは取得できない。
            {
                QuestSystem qs;
                bool ret = qs.Init(_T("..\\UnitTest1\\sample18.csv"), _T(""), false);
                std::wstring result = qs.GetQuestIdFinishByExamine(0.f, 0.f, 0.f);
                Assert::AreEqual((int)result.size(), 0);
            }
        }

        // テストしたいこと
        // 「クエストが完了していたら」が複数指定されていた時にクエストが開始できるか
        TEST_METHOD(TestMethod19)
        {
            QuestSystem qs;
            bool ret = qs.Init(_T("..\\UnitTest1\\sample19.csv"), _T(""), false);

            qs.SetTalk(_T("きんにくん"));
            qs.SetTalk(_T("A"));
            qs.SetTalk(_T("B"));
            qs.SetTalk(_T("C"));
            qs.SetTalk(_T("D"));
            qs.SetTalk(_T("E"));

            std::vector<std::wstring> vs;

            vs = qs.GetStartQuest();
            Assert::AreEqual((size_t)1, vs.size());
            Assert::AreEqual(std::wstring(_T("Q7")), vs.at(0));
        }

        // テストしたいこと
        // 「クエストが完了していたら」が複数指定されていた時にクエストが完了できるか
        TEST_METHOD(TestMethod20)
        {
            QuestSystem qs;
            bool ret = qs.Init(_T("..\\UnitTest1\\sample20.csv"), _T(""), false);

            qs.SetTalk(_T("きんにくん"));
            qs.SetTalk(_T("A"));
            qs.SetTalk(_T("B"));
            qs.SetTalk(_T("C"));
            qs.SetTalk(_T("D"));
            qs.SetTalk(_T("E"));
            qs.SetTalk(_T("F"));
            qs.SetTalk(_T("G"));
            qs.SetTalk(_T("H"));
            qs.SetTalk(_T("I"));
            qs.SetTalk(_T("J"));
            qs.SetTalk(_T("K"));

            std::vector<std::wstring> vs;
            vs = qs.GetFinishQuest();

            Assert::AreEqual((size_t)7, vs.size());
            auto it = std::find(vs.begin(), vs.end(), _T("Q7"));

            Assert::AreEqual(true, it != vs.end());
        }

        // テストしたいこと
        // 「一つでもクエストが完了していたら」が動作すること
        TEST_METHOD(TestMethod21)
        {
            QuestSystem qs;
            bool ret = qs.Init(_T("..\\UnitTest1\\sample21.csv"), _T(""), false);

            qs.SetTalk(_T("B"));
            qs.SetTalk(_T("C"));

            std::vector<std::wstring> vs;
            vs = qs.GetStartQuest();

            Assert::AreEqual((size_t)1, vs.size());
            auto it = std::find(vs.begin(), vs.end(), _T("Q7"));

            Assert::AreEqual(true, it != vs.end());
        }

//        // テストしたいこと
//        // 暗号化されたファイルを開けること
//        TEST_METHOD(TestMethod22)
//        {
//            QuestSystem qs;
//            bool ret = qs.Init(_T("..\\UnitTest1\\sample22.enc"), _T(""), true);
//
//            qs.SetTalk(_T("B"));
//            qs.SetTalk(_T("C"));
//
//            std::vector<std::wstring> vs;
//            vs = qs.GetStartQuest();
//
//            Assert::AreEqual((size_t)1, vs.size());
//            auto it = std::find(vs.begin(), vs.end(), _T("Q7"));
//
//            Assert::AreEqual(true, it != vs.end());
//        }

        // テストしたいこと
        // 保存されたクエストデータを読み、途中からスタートできること。
        TEST_METHOD(TestMethod23)
        {
            QuestSystem qs;
            bool ret = qs.Init(_T("..\\UnitTest1\\sample23.csv"), _T("..\\UnitTest1\\sample23Save.csv"), false);

            qs.SetTalk(_T("D"));
            qs.SetTalk(_T("E"));

            std::vector<std::wstring> vs;
            vs = qs.GetStartQuest();

            Assert::AreEqual((size_t)1, vs.size());
            auto it = std::find(vs.begin(), vs.end(), _T("Q7"));

            Assert::AreEqual(true, it != vs.end());
        }

        // テストしたいこと
        // インベントリのアイテムの個数によってクエストがスタートしたり終了したりすること
        TEST_METHOD(TestMethod24)
        {
            {
                QuestSystem qs;
                bool ret = qs.Init(_T("..\\UnitTest1\\sample24.csv"), _T(""), false);
                std::vector<ItemInfo> vs;
                qs.SetInventoryContent(vs);

                std::vector<std::wstring> vs2;
                vs2 = qs.GetStartQuest();

                Assert::AreEqual((size_t)0, vs2.size());
            }
            {
                QuestSystem qs;
                bool ret = qs.Init(_T("..\\UnitTest1\\sample24.csv"), _T(""), false);
                std::vector<ItemInfo> vs;
                ItemInfo itemInfo;
                itemInfo.m_itemName = _T("トマト");
                vs.push_back(itemInfo);

                qs.SetInventoryContent(vs);

                std::vector<std::wstring> vs2;
                vs2 = qs.GetStartQuest();

                Assert::AreEqual((size_t)0, vs2.size());
            }
            {
                QuestSystem qs;
                bool ret = qs.Init(_T("..\\UnitTest1\\sample24.csv"), _T(""), false);
                std::vector<ItemInfo> vs;
                ItemInfo itemInfo;
                itemInfo.m_itemName = _T("トマト");
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);

                qs.SetInventoryContent(vs);

                std::vector<std::wstring> vs2;
                vs2 = qs.GetStartQuest();

                Assert::AreEqual((size_t)0, vs2.size());
            }
            {
                QuestSystem qs;
                bool ret = qs.Init(_T("..\\UnitTest1\\sample24.csv"), _T(""), false);
                std::vector<ItemInfo> vs;
                ItemInfo itemInfo;
                itemInfo.m_itemName = _T("トマト");
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);

                qs.SetInventoryContent(vs);

                std::vector<std::wstring> vs2;
                vs2 = qs.GetStartQuest();

                Assert::AreEqual((size_t)1, vs2.size());

                auto it = std::find(vs2.begin(), vs2.end(), _T("Q1"));
                Assert::AreEqual(true, it != vs2.end());
            }
            {
                QuestSystem qs;
                bool ret = qs.Init(_T("..\\UnitTest1\\sample24.csv"), _T(""), false);
                std::vector<ItemInfo> vs;
                ItemInfo itemInfo;
                itemInfo.m_itemName = _T("バナナ");
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);

                qs.SetInventoryContent(vs);

                std::vector<std::wstring> vs2;
                vs2 = qs.GetStartQuest();

                Assert::AreEqual((size_t)0, vs2.size());
            }
            {
                QuestSystem qs;
                bool ret = qs.Init(_T("..\\UnitTest1\\sample24.csv"), _T(""), false);
                std::vector<ItemInfo> vs;
                ItemInfo itemInfo;
                itemInfo.m_itemName = _T("バナナ");
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);

                qs.SetInventoryContent(vs);

                std::vector<std::wstring> vs2;
                vs2 = qs.GetStartQuest();
                Assert::AreEqual((size_t)0, vs2.size());

                vs2 = qs.GetFinishQuest();
                Assert::AreEqual((size_t)0, vs2.size());
            }
            {
                QuestSystem qs;
                bool ret = qs.Init(_T("..\\UnitTest1\\sample24.csv"), _T(""), false);
                std::vector<ItemInfo> vs;
                ItemInfo itemInfo;
                itemInfo.m_itemName = _T("トマト");
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);

                qs.SetInventoryContent(vs);

                std::vector<std::wstring> vs2;
                vs2 = qs.GetStartQuest();
                Assert::AreEqual((size_t)1, vs2.size());

                itemInfo.m_itemName = _T("バナナ");
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);

                qs.SetInventoryContent(vs);

                vs2 = qs.GetFinishQuest();
                Assert::AreEqual((size_t)1, vs2.size());
            }
            {
                QuestSystem qs;
                bool ret = qs.Init(_T("..\\UnitTest1\\sample24.csv"), _T(""), false);
                std::vector<ItemInfo> vs;
                ItemInfo itemInfo;
                itemInfo.m_itemName = _T("トマト");
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);

                itemInfo.m_itemName = _T("バナナ");
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);

                qs.SetInventoryContent(vs);

                std::vector<std::wstring> vs2;
                vs2 = qs.GetStartQuest();
                Assert::AreEqual((size_t)1, vs2.size());

                // クエスト開始時に、すでに完了できる状態だったときは
                // もう一度SetInventoryContentを呼ばないと完了できないこととする。
                vs2 = qs.GetFinishQuest();
                Assert::AreEqual((size_t)0, vs2.size());

                //
                qs.SetInventoryContent(vs);

                vs2 = qs.GetFinishQuest();
                Assert::AreEqual((size_t)1, vs2.size());
            }
        }

        // テストしたいこと
        // 倉庫のアイテムの個数によってクエストがスタートしたり終了したりすること
        TEST_METHOD(TestMethod25)
        {
            {
                QuestSystem qs;
                bool ret = qs.Init(_T("..\\UnitTest1\\sample25.csv"), _T(""), false);
                std::vector<ItemInfo> vs;
                qs.SetStorehouseContent(1, vs);

                std::vector<std::wstring> vs2;
                vs2 = qs.GetStartQuest();

                Assert::AreEqual((size_t)0, vs2.size());
            }
            {
                QuestSystem qs;
                bool ret = qs.Init(_T("..\\UnitTest1\\sample25.csv"), _T(""), false);
                std::vector<ItemInfo> vs;
                ItemInfo itemInfo;
                itemInfo.m_itemName = _T("トマト");
                vs.push_back(itemInfo);

                qs.SetStorehouseContent(1, vs);

                std::vector<std::wstring> vs2;
                vs2 = qs.GetStartQuest();

                Assert::AreEqual((size_t)0, vs2.size());
            }
            {
                QuestSystem qs;
                bool ret = qs.Init(_T("..\\UnitTest1\\sample25.csv"), _T(""), false);
                std::vector<ItemInfo> vs;
                ItemInfo itemInfo;
                itemInfo.m_itemName = _T("トマト");
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);

                qs.SetStorehouseContent(1, vs);

                std::vector<std::wstring> vs2;
                vs2 = qs.GetStartQuest();

                Assert::AreEqual((size_t)0, vs2.size());
            }
            {
                QuestSystem qs;
                bool ret = qs.Init(_T("..\\UnitTest1\\sample25.csv"), _T(""), false);
                std::vector<ItemInfo> vs;
                ItemInfo itemInfo;
                itemInfo.m_itemName = _T("トマト");
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);

                qs.SetStorehouseContent(1, vs);

                std::vector<std::wstring> vs2;
                vs2 = qs.GetStartQuest();

                Assert::AreEqual((size_t)1, vs2.size());

                auto it = std::find(vs2.begin(), vs2.end(), _T("Q1"));
                Assert::AreEqual(true, it != vs2.end());
            }
            {
                QuestSystem qs;
                bool ret = qs.Init(_T("..\\UnitTest1\\sample25.csv"), _T(""), false);
                std::vector<ItemInfo> vs;
                ItemInfo itemInfo;
                itemInfo.m_itemName = _T("バナナ");
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);

                qs.SetStorehouseContent(1, vs);

                std::vector<std::wstring> vs2;
                vs2 = qs.GetStartQuest();

                Assert::AreEqual((size_t)0, vs2.size());
            }
            {
                QuestSystem qs;
                bool ret = qs.Init(_T("..\\UnitTest1\\sample25.csv"), _T(""), false);
                std::vector<ItemInfo> vs;
                ItemInfo itemInfo;
                itemInfo.m_itemName = _T("バナナ");
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);

                qs.SetStorehouseContent(1, vs);

                std::vector<std::wstring> vs2;
                vs2 = qs.GetStartQuest();
                Assert::AreEqual((size_t)0, vs2.size());

                vs2 = qs.GetFinishQuest();
                Assert::AreEqual((size_t)0, vs2.size());
            }
            {
                QuestSystem qs;
                bool ret = qs.Init(_T("..\\UnitTest1\\sample25.csv"), _T(""), false);
                std::vector<ItemInfo> vs;
                ItemInfo itemInfo;
                itemInfo.m_itemName = _T("トマト");
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);

                qs.SetStorehouseContent(1, vs);

                std::vector<std::wstring> vs2;
                vs2 = qs.GetStartQuest();
                Assert::AreEqual((size_t)1, vs2.size());

                itemInfo.m_itemName = _T("バナナ");
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);

                qs.SetStorehouseContent(1, vs);

                vs2 = qs.GetFinishQuest();
                Assert::AreEqual((size_t)1, vs2.size());
            }
            {
                QuestSystem qs;
                bool ret = qs.Init(_T("..\\UnitTest1\\sample25.csv"), _T(""), false);
                std::vector<ItemInfo> vs;
                ItemInfo itemInfo;
                itemInfo.m_itemName = _T("トマト");
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);

                itemInfo.m_itemName = _T("バナナ");
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);

                qs.SetStorehouseContent(1, vs);

                std::vector<std::wstring> vs2;
                vs2 = qs.GetStartQuest();
                Assert::AreEqual((size_t)1, vs2.size());

                // クエスト開始時に、すでに完了できる状態だったときは
                // もう一度SetStorehouseContentを呼ばないと完了できないこととする。
                vs2 = qs.GetFinishQuest();
                Assert::AreEqual((size_t)0, vs2.size());

                //
                qs.SetStorehouseContent(1, vs);

                vs2 = qs.GetFinishQuest();
                Assert::AreEqual((size_t)1, vs2.size());
            }
        }

        // テストしたいこと
        // インベントリのアイテムの個数によってクエストがスタートしたり終了したりすること
        // その際に、強化値も影響すること
        TEST_METHOD(TestMethod26)
        {
            {
                QuestSystem qs;
                bool ret = qs.Init(_T("..\\UnitTest1\\sample26.csv"), _T(""), false);
                std::vector<ItemInfo> vs;
                qs.SetInventoryContent(vs);

                std::vector<std::wstring> vs2;
                vs2 = qs.GetStartQuest();

                Assert::AreEqual((size_t)0, vs2.size());
            }
            {
                QuestSystem qs;
                bool ret = qs.Init(_T("..\\UnitTest1\\sample26.csv"), _T(""), false);
                std::vector<ItemInfo> vs;
                ItemInfo itemInfo;
                itemInfo.m_itemName = _T("トマト");
                vs.push_back(itemInfo);

                qs.SetInventoryContent(vs);

                std::vector<std::wstring> vs2;
                vs2 = qs.GetStartQuest();

                Assert::AreEqual((size_t)0, vs2.size());
            }
            {
                QuestSystem qs;
                bool ret = qs.Init(_T("..\\UnitTest1\\sample26.csv"), _T(""), false);
                std::vector<ItemInfo> vs;
                ItemInfo itemInfo;
                itemInfo.m_itemName = _T("トマト");
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);

                qs.SetInventoryContent(vs);

                std::vector<std::wstring> vs2;
                vs2 = qs.GetStartQuest();

                Assert::AreEqual((size_t)0, vs2.size());
            }
            {
                QuestSystem qs;
                bool ret = qs.Init(_T("..\\UnitTest1\\sample26.csv"), _T(""), false);
                std::vector<ItemInfo> vs;
                ItemInfo itemInfo;
                itemInfo.m_itemName = _T("トマト");
                itemInfo.m_level = 3;
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);

                qs.SetInventoryContent(vs);

                std::vector<std::wstring> vs2;
                vs2 = qs.GetStartQuest();

                Assert::AreEqual((size_t)1, vs2.size());

                auto it = std::find(vs2.begin(), vs2.end(), _T("Q1"));
                Assert::AreEqual(true, it != vs2.end());
            }
            {
                QuestSystem qs;
                bool ret = qs.Init(_T("..\\UnitTest1\\sample26.csv"), _T(""), false);
                std::vector<ItemInfo> vs;
                ItemInfo itemInfo;
                itemInfo.m_itemName = _T("バナナ");
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);

                qs.SetInventoryContent(vs);

                std::vector<std::wstring> vs2;
                vs2 = qs.GetStartQuest();

                Assert::AreEqual((size_t)0, vs2.size());
            }
            {
                QuestSystem qs;
                bool ret = qs.Init(_T("..\\UnitTest1\\sample26.csv"), _T(""), false);
                std::vector<ItemInfo> vs;
                ItemInfo itemInfo;
                itemInfo.m_itemName = _T("バナナ");
                itemInfo.m_level = 4;
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);

                qs.SetInventoryContent(vs);

                std::vector<std::wstring> vs2;
                vs2 = qs.GetStartQuest();
                Assert::AreEqual((size_t)0, vs2.size());

                vs2 = qs.GetFinishQuest();
                Assert::AreEqual((size_t)0, vs2.size());
            }
            {
                QuestSystem qs;
                bool ret = qs.Init(_T("..\\UnitTest1\\sample26.csv"), _T(""), false);
                std::vector<ItemInfo> vs;
                ItemInfo itemInfo;
                itemInfo.m_itemName = _T("トマト");
                itemInfo.m_level = 3;
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);

                qs.SetInventoryContent(vs);

                std::vector<std::wstring> vs2;
                vs2 = qs.GetStartQuest();
                Assert::AreEqual((size_t)1, vs2.size());

                itemInfo.m_itemName = _T("バナナ");
                itemInfo.m_level = 4;
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);

                qs.SetInventoryContent(vs);

                vs2 = qs.GetFinishQuest();
                Assert::AreEqual((size_t)1, vs2.size());
            }
            {
                QuestSystem qs;
                bool ret = qs.Init(_T("..\\UnitTest1\\sample26.csv"), _T(""), false);
                std::vector<ItemInfo> vs;
                ItemInfo itemInfo;
                itemInfo.m_itemName = _T("トマト");
                itemInfo.m_level = 3;
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);

                itemInfo.m_itemName = _T("バナナ");
                itemInfo.m_level = 4;
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);

                qs.SetInventoryContent(vs);

                std::vector<std::wstring> vs2;
                vs2 = qs.GetStartQuest();
                Assert::AreEqual((size_t)1, vs2.size());

                // クエスト開始時に、すでに完了できる状態だったときは
                // もう一度SetInventoryContentを呼ばないと完了できないこととする。
                vs2 = qs.GetFinishQuest();
                Assert::AreEqual((size_t)0, vs2.size());

                //
                qs.SetInventoryContent(vs);

                vs2 = qs.GetFinishQuest();
                Assert::AreEqual((size_t)1, vs2.size());
            }
        }

        // テストしたいこと
        // 倉庫のアイテムの個数によってクエストがスタートしたり終了したりすること
        // その際に、強化値も影響すること
        TEST_METHOD(TestMethod27)
        {
            {
                QuestSystem qs;
                bool ret = qs.Init(_T("..\\UnitTest1\\sample27.csv"), _T(""), false);
                std::vector<ItemInfo> vs;
                qs.SetStorehouseContent(1, vs);

                std::vector<std::wstring> vs2;
                vs2 = qs.GetStartQuest();

                Assert::AreEqual((size_t)0, vs2.size());
            }
            {
                QuestSystem qs;
                bool ret = qs.Init(_T("..\\UnitTest1\\sample27.csv"), _T(""), false);
                std::vector<ItemInfo> vs;
                ItemInfo itemInfo;
                itemInfo.m_itemName = _T("トマト");
                vs.push_back(itemInfo);

                qs.SetStorehouseContent(1, vs);

                std::vector<std::wstring> vs2;
                vs2 = qs.GetStartQuest();

                Assert::AreEqual((size_t)0, vs2.size());
            }
            {
                QuestSystem qs;
                bool ret = qs.Init(_T("..\\UnitTest1\\sample27.csv"), _T(""), false);
                std::vector<ItemInfo> vs;
                ItemInfo itemInfo;
                itemInfo.m_itemName = _T("トマト");
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);

                qs.SetStorehouseContent(1, vs);

                std::vector<std::wstring> vs2;
                vs2 = qs.GetStartQuest();

                Assert::AreEqual((size_t)0, vs2.size());
            }
            {
                QuestSystem qs;
                bool ret = qs.Init(_T("..\\UnitTest1\\sample27.csv"), _T(""), false);
                std::vector<ItemInfo> vs;
                ItemInfo itemInfo;
                itemInfo.m_itemName = _T("トマト");
                itemInfo.m_level = 3;
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);

                qs.SetStorehouseContent(1, vs);

                std::vector<std::wstring> vs2;
                vs2 = qs.GetStartQuest();

                Assert::AreEqual((size_t)1, vs2.size());

                auto it = std::find(vs2.begin(), vs2.end(), _T("Q1"));
                Assert::AreEqual(true, it != vs2.end());
            }
            {
                QuestSystem qs;
                bool ret = qs.Init(_T("..\\UnitTest1\\sample27.csv"), _T(""), false);
                std::vector<ItemInfo> vs;
                ItemInfo itemInfo;
                itemInfo.m_itemName = _T("バナナ");
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);

                qs.SetStorehouseContent(1, vs);

                std::vector<std::wstring> vs2;
                vs2 = qs.GetStartQuest();

                Assert::AreEqual((size_t)0, vs2.size());
            }
            {
                QuestSystem qs;
                bool ret = qs.Init(_T("..\\UnitTest1\\sample27.csv"), _T(""), false);
                std::vector<ItemInfo> vs;
                ItemInfo itemInfo;
                itemInfo.m_itemName = _T("バナナ");
                itemInfo.m_level = 4;
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);

                qs.SetStorehouseContent(1, vs);

                std::vector<std::wstring> vs2;
                vs2 = qs.GetStartQuest();
                Assert::AreEqual((size_t)0, vs2.size());

                vs2 = qs.GetFinishQuest();
                Assert::AreEqual((size_t)0, vs2.size());
            }
            {
                QuestSystem qs;
                bool ret = qs.Init(_T("..\\UnitTest1\\sample27.csv"), _T(""), false);
                std::vector<ItemInfo> vs;
                ItemInfo itemInfo;
                itemInfo.m_itemName = _T("トマト");
                itemInfo.m_level = 3;
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);

                qs.SetStorehouseContent(1, vs);

                std::vector<std::wstring> vs2;
                vs2 = qs.GetStartQuest();
                Assert::AreEqual((size_t)1, vs2.size());

                itemInfo.m_itemName = _T("バナナ");
                itemInfo.m_level = 4;
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);

                qs.SetStorehouseContent(1, vs);

                vs2 = qs.GetFinishQuest();
                Assert::AreEqual((size_t)1, vs2.size());
            }
            {
                QuestSystem qs;
                bool ret = qs.Init(_T("..\\UnitTest1\\sample27.csv"), _T(""), false);
                std::vector<ItemInfo> vs;
                ItemInfo itemInfo;
                itemInfo.m_itemName = _T("トマト");
                itemInfo.m_level = 3;
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);

                itemInfo.m_itemName = _T("バナナ");
                itemInfo.m_level = 4;
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);
                vs.push_back(itemInfo);

                qs.SetStorehouseContent(1, vs);

                std::vector<std::wstring> vs2;
                vs2 = qs.GetStartQuest();
                Assert::AreEqual((size_t)1, vs2.size());

                // クエスト開始時に、すでに完了できる状態だったときは
                // もう一度SetStorehouseContentを呼ばないと完了できないこととする。
                vs2 = qs.GetFinishQuest();
                Assert::AreEqual((size_t)0, vs2.size());

                //
                qs.SetStorehouseContent(1, vs);

                vs2 = qs.GetFinishQuest();
                Assert::AreEqual((size_t)1, vs2.size());
            }
        }

        // テストしたいこと
        // 完了したクエストが、再スタートしないこと
        TEST_METHOD(TestMethod28)
        {
            QuestSystem qs;
            bool ret = qs.Init(_T("..\\UnitTest1\\sample28.csv"), _T("..\\UnitTest1\\sample28Save.csv"), false);

            qs.SetPos(-285.f, 16.f, 539.f);
            qs.SetPos(-285.f, 16.f, 539.f);

            std::vector<std::wstring> vs;
            vs = qs.GetStartQuest();

            Assert::AreEqual((size_t)0, vs.size());

            vs = qs.GetFinishQuest();

            Assert::AreEqual((size_t)0, vs.size());
        }

        // テストしたいこと
        // 体の体力が指定値より少なければクエストが開始すること
        TEST_METHOD(TestMethod29)
        {
            QuestSystem qs;
            bool ret = qs.Init(_T("..\\UnitTest1\\sample29.csv"), _T(""), false);

            qs.SetBodyStamina(95);

            std::vector<std::wstring> vs;
            vs = qs.GetStartQuest();

            Assert::AreEqual<size_t>(1, vs.size());
            Assert::AreEqual(_T("Q1"), vs.at(0).c_str());
        }

        // テストしたいこと
        // 脳の体力が指定値より少なければクエストが開始すること
        TEST_METHOD(TestMethod30)
        {
            QuestSystem qs;
            bool ret = qs.Init(_T("..\\UnitTest1\\sample29.csv"), _T(""), false);

            qs.SetBrainStamina(75);

            std::vector<std::wstring> vs;
            vs = qs.GetStartQuest();

            Assert::AreNotEqual<size_t>(0, vs.size());
            auto it = std::find(vs.begin(), vs.end(), _T("Q2"));
            Assert::IsTrue(it != vs.end());
        }

        // テストしたいこと
        // 体の体力が指定値より少なければクエストが完了すること
        TEST_METHOD(TestMethod31)
        {
            QuestSystem qs;
            {
                bool ret = qs.Init(_T("..\\UnitTest1\\sample29.csv"), _T(""), false);
                qs.SetBodyStamina(55);
                std::vector<std::wstring> vs;
                vs = qs.GetStartQuest();
                Assert::AreNotEqual<size_t>(0, vs.size());
                auto it = std::find(vs.begin(), vs.end(), _T("Q3"));
                Assert::IsTrue(it != vs.end());
            }

            qs.SetBodyStamina(45);

            {
                std::vector<std::wstring> vs;
                vs = qs.GetFinishQuest();
                Assert::AreNotEqual<size_t>(0, vs.size());
                auto it = std::find(vs.begin(), vs.end(), _T("Q3"));
                Assert::IsTrue(it != vs.end());
            }
        }

        // テストしたいこと
        // 脳の体力が指定値より少なければクエストが完了すること
        TEST_METHOD(TestMethod32)
        {
            QuestSystem qs;
            {
                bool ret = qs.Init(_T("..\\UnitTest1\\sample29.csv"), _T(""), false);
                qs.SetBodyStamina(35);
                std::vector<std::wstring> vs;
                vs = qs.GetStartQuest();
                Assert::AreNotEqual<size_t>(0, vs.size());
                auto it = std::find(vs.begin(), vs.end(), _T("Q4"));
                Assert::IsTrue(it != vs.end());
            }

            qs.SetBrainStamina(25);

            {
                std::vector<std::wstring> vs;
                vs = qs.GetFinishQuest();
                Assert::AreNotEqual<size_t>(0, vs.size());
                auto it = std::find(vs.begin(), vs.end(), _T("Q4"));
                Assert::IsTrue(it != vs.end());
            }
        }

        // テストしたいこと
        // 位置が範囲外だったらクエストが開始すること1
        TEST_METHOD(TestMethod33)
        {
            QuestSystem qs;
            bool ret = qs.Init(_T("..\\UnitTest1\\sample30.csv"), _T(""), false);
            qs.SetPos(1.f, 2.f, 3.f);
            std::vector<std::wstring> vs;
            vs = qs.GetStartQuest();

            // 範囲内だったらクエストが開始しない
            Assert::AreEqual<size_t>(0, vs.size());

            qs.SetPos(50.f, 2.f, 3.f);
            vs = qs.GetStartQuest();

            // 範囲外だったらクエストが開始する
            Assert::AreNotEqual<size_t>(0, vs.size());
            auto it = std::find(vs.begin(), vs.end(), _T("Q1"));
            Assert::IsTrue(it != vs.end());
        }

        // テストしたいこと
        // 位置が範囲外だったらクエストが完了すること2
        TEST_METHOD(TestMethod34)
        {
            QuestSystem qs;
            bool ret = qs.Init(_T("..\\UnitTest1\\sample30.csv"), _T(""), false);
            qs.SetPos(120.f, 200.f, 320.f);
            std::vector<std::wstring> vs;
            vs = qs.GetFinishQuest();

            // 範囲内だったらクエストが完了しない
            Assert::AreEqual<size_t>(0, vs.size());

            qs.SetPos(10.f, 20.f, 30.f);
            vs = qs.GetFinishQuest();

            // 範囲外だったらクエストが完了する
            Assert::AreNotEqual<size_t>(0, vs.size());
            auto it = std::find(vs.begin(), vs.end(), _T("Q1"));
            Assert::IsTrue(it != vs.end());
        }

        // テストしたいこと
        // 時間経過でテストが完了すること
        TEST_METHOD(TestMethod35)
        {
            QuestSystem qs;
            std::vector<std::wstring> vs;

            // 1か月と2日と3時間4分5秒経過したら完了するクエスト
            bool ret = qs.Init(_T("..\\UnitTest1\\sample35.csv"), _T(""), false);
            qs.SetCurrentDateTime(1, 2, 3, 4, 5, 6);
            qs.SetPos(100.f, 0.f, 0.f);
            vs = qs.GetStartQuest();

            Assert::AreEqual<size_t>(1, vs.size());

            qs.SetCurrentDateTime(1, 3, 5, 7, 9, 7);
            vs = qs.GetFinishQuest();

            Assert::AreEqual<size_t>(0, vs.size());

            qs.SetCurrentDateTime(1, 3, 5, 7, 9, 11);
            vs = qs.GetFinishQuest();

            Assert::AreEqual<size_t>(1, vs.size());

            auto it = std::find(vs.begin(), vs.end(), _T("Q1"));
            Assert::IsTrue(it != vs.end());
        }

        // テストしたいこと
        // 昼だったらテストが完了すること
        TEST_METHOD(TestMethod36)
        {
            QuestSystem qs;
            std::vector<std::wstring> vs;

            // 1か月と2日と3時間4分5秒経過したら完了するクエスト
            bool ret = qs.Init(_T("..\\UnitTest1\\sample35.csv"), _T(""), false);
            qs.SetCurrentDateTime(1, 2, 3, 4, 5, 6);
            qs.SetPos(200.f, 0.f, 0.f);
            vs = qs.GetStartQuest();

            Assert::AreEqual<size_t>(1, vs.size());

            qs.SetCurrentDateTime(1, 2, 3, 5, 59, 59);
            vs = qs.GetFinishQuest();

            Assert::AreEqual<size_t>(0, vs.size());

            qs.SetCurrentDateTime(1, 3, 5, 6, 0, 0);
            vs = qs.GetFinishQuest();

            Assert::AreEqual<size_t>(1, vs.size());

            auto it = std::find(vs.begin(), vs.end(), _T("Q2"));
            Assert::IsTrue(it != vs.end());
        }

        // テストしたいこと
        // 夜だったらテストが完了すること
        TEST_METHOD(TestMethod37)
        {
            QuestSystem qs;
            std::vector<std::wstring> vs;

            // 1か月と2日と3時間4分5秒経過したら完了するクエスト
            bool ret = qs.Init(_T("..\\UnitTest1\\sample35.csv"), _T(""), false);
            qs.SetCurrentDateTime(1, 2, 3, 4, 5, 6);
            qs.SetPos(300.f, 0.f, 0.f);
            vs = qs.GetStartQuest();

            qs.SetCurrentDateTime(1, 3, 5, 17, 59, 59);
            Assert::AreEqual<size_t>(1, vs.size());

            vs = qs.GetFinishQuest();

            Assert::AreEqual<size_t>(0, vs.size());

            qs.SetCurrentDateTime(1, 2, 3, 18, 0, 0);
            vs = qs.GetFinishQuest();

            Assert::AreEqual<size_t>(1, vs.size());

            auto it = std::find(vs.begin(), vs.end(), _T("Q3"));
            Assert::IsTrue(it != vs.end());
        }

        // テストしたいこと
        // 時間経過でテストが完了すること2(23時59分59秒から開始)
        TEST_METHOD(TestMethod38)
        {
            QuestSystem qs;
            std::vector<std::wstring> vs;

            // 1か月と2日と3時間4分5秒経過したら完了するクエスト
            // 1年2月3日23時59分59秒の1か月と2日と3時間4分5秒後は
            // 1年3月6日0時4分4秒
            bool ret = qs.Init(_T("..\\UnitTest1\\sample35.csv"), _T(""), false);
            qs.SetCurrentDateTime(1, 2, 3, 23, 59, 59);
            qs.SetPos(100.f, 0.f, 0.f);
            vs = qs.GetStartQuest();

            Assert::AreEqual<size_t>(1, vs.size());

            qs.SetCurrentDateTime(1, 3, 6, 3, 4, 3);
            vs = qs.GetFinishQuest();

            Assert::AreEqual<size_t>(0, vs.size());

            qs.SetCurrentDateTime(1, 3, 6, 3, 4, 4);
            vs = qs.GetFinishQuest();

            Assert::AreEqual<size_t>(1, vs.size());

            auto it = std::find(vs.begin(), vs.end(), _T("Q1"));
            Assert::IsTrue(it != vs.end());
        }

        // テストしたいこと
        // クエスト完了条件のオプションが複数行あり、その中に空行があっても正しく処理されるか
        TEST_METHOD(TestMethod39)
        {
            QuestSystem qs;
            std::vector<std::wstring> vs;

            bool ret = qs.Init(_T("..\\UnitTest1\\sample39.csv"), _T(""), false);
            qs.SetPos(100.f, 0.f, 0.f);
            vs = qs.GetStartQuest();

            Assert::AreEqual<size_t>(1, vs.size());

            qs.SetExamine(0.f, 0.f, 0.f);
            vs = qs.GetFinishQuest();

            Assert::AreEqual<size_t>(1, vs.size());

            qs.SetPos(200.f, 0.f, 0.f);
            vs = qs.GetStartQuest();

            Assert::AreEqual<size_t>(1, vs.size());

            qs.SetTalk(L"太郎");
            qs.SetTalk(L"三郎");
            qs.SetCurrentDateTime(1,2,3,4,5,6);
            vs = qs.GetFinishQuest();

            Assert::AreEqual<size_t>(1, vs.size());
        }
    };
}
