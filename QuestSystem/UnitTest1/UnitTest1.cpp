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
	};
}
