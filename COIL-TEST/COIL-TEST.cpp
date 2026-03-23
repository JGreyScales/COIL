#include "pch.h"
#include "CppUnitTest.h"
#include <bitset>


#include "HeaderDef.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace COILTEST
{
	TEST_CLASS(HeaderDefTests)
	{
	public:
		
		TEST_METHOD(sanityInstanceCreation)
		{
			HeaderDef* HeaderInstance = new HeaderDef();

			uint8_t pktCount = 10;
			uint8_t padding = 2;
			uint8_t ack = true;
			uint8_t sleep = false;
			uint8_t status = true;
			uint8_t drive = false;
			uint8_t length = 50;

			HeaderInstance->PktCount = pktCount;
			HeaderInstance->Padding = padding;
			HeaderInstance->Ack = ack;
			HeaderInstance->Sleep = sleep;
			HeaderInstance->Status = status;
			HeaderInstance->Drive = drive;
			HeaderInstance->Length = length;

			char* serialData = HeaderInstance->serialize();
			HeaderDef* deserializedClass = HeaderInstance->deserialize(serialData);

			Assert::AreEqual(pktCount, deserializedClass->PktCount);
			Assert::AreEqual(padding, deserializedClass->Padding);
			Assert::AreEqual(ack, deserializedClass->Ack);
			Assert::AreEqual(sleep, deserializedClass->Sleep);
			Assert::AreEqual(status, deserializedClass->Status);
			Assert::AreEqual(drive, deserializedClass->Drive);
			Assert::AreEqual(length, deserializedClass->Length);


			delete HeaderInstance;
			delete deserializedClass;
		}

		TEST_METHOD(headerBinaryLength) {
			Assert::AreEqual((size_t)3, sizeof(HeaderDef));
		}

		TEST_METHOD(HeaderBinaryOrder) {
			HeaderDef* HeaderInstance = new HeaderDef();

			HeaderInstance->PktCount = 10;
			HeaderInstance->Sleep = true;
			HeaderInstance->Length = 10;

			// expected binary
			// 00000001
			// 01000000
			// 00001010

			uint32_t value = 0;
			memcpy(&value, HeaderInstance, 3);
			std::bitset<24> rawData(value);

			Assert::IsTrue(rawData[7]);
			Assert::IsTrue(rawData[9]);
			Assert::IsTrue(rawData[20]);
			Assert::IsTrue(rawData[22]);

		}
	};
}
