#include "pch.h"
#include "CppUnitTest.h"
#include <bitset>


#include "HeaderDef.h"
#include "TelemetryBodyDef.h"

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
			Assert::AreEqual(3, HeaderDef::HEADER_SIZE);
		}

		TEST_METHOD(HeaderBinaryOrder) {
			HeaderDef* HeaderInstance = new HeaderDef();

			HeaderInstance->PktCount = 1;
			HeaderInstance->Sleep = true;
			HeaderInstance->Drive = true;
			HeaderInstance->Length = 10;

			char* value = HeaderInstance->serialize();
			std::bitset<8> byte2(value[1]);
			
			Assert::AreEqual<uint8_t>(1, (uint8_t)value[0]);
			Assert::AreEqual<uint8_t>(5, (uint8_t)value[1]);
			Assert::AreEqual<uint8_t>(10, (uint8_t)value[2]);
		}
	};

	TEST_CLASS(TelemetryBodyDefTests)
	{
	public:

		TEST_METHOD(DefaultConstructor_SetsAllFieldsToZero)
		{
			TelemetryBodyDef body;

			Assert::AreEqual((unsigned short)0, body.LastPktCounter);
			Assert::AreEqual((unsigned short)0, body.CurrentGrade);
			Assert::AreEqual((unsigned short)0, body.HitCount);
			Assert::AreEqual((unsigned short)0, body.Heading);
			Assert::AreEqual((unsigned char)0, body.LastCmd);
			Assert::AreEqual((unsigned char)0, body.LastCmdValue);
			Assert::AreEqual((unsigned char)0, body.LastCmdPower);
		}

		TEST_METHOD(ParameterizedConstructor_SetsAllFieldsCorrectly)
		{
			TelemetryBodyDef body(25, 92, 3, 180, 1, 7, 95);

			Assert::AreEqual((unsigned short)25, body.LastPktCounter);
			Assert::AreEqual((unsigned short)92, body.CurrentGrade);
			Assert::AreEqual((unsigned short)3, body.HitCount);
			Assert::AreEqual((unsigned short)180, body.Heading);
			Assert::AreEqual((unsigned char)1, body.LastCmd);
			Assert::AreEqual((unsigned char)7, body.LastCmdValue);
			Assert::AreEqual((unsigned char)95, body.LastCmdPower);
		}

		TEST_METHOD(SerializeAndDeserialize_PreserveExactValues)
		{
			TelemetryBodyDef original(100, 88, 6, 270, 4, 5, 100);

			char* raw = original.serialize();
			TelemetryBodyDef* restored = TelemetryBodyDef::deserialize(raw);

			Assert::AreEqual(original.LastPktCounter, restored->LastPktCounter);
			Assert::AreEqual(original.CurrentGrade, restored->CurrentGrade);
			Assert::AreEqual(original.HitCount, restored->HitCount);
			Assert::AreEqual(original.Heading, restored->Heading);
			Assert::AreEqual(original.LastCmd, restored->LastCmd);
			Assert::AreEqual(original.LastCmdValue, restored->LastCmdValue);
			Assert::AreEqual(original.LastCmdPower, restored->LastCmdPower);

			delete[] raw;
			delete restored;
		}

		TEST_METHOD(Deserialize_WithNullBuffer_ThrowsInvalidArgument)
		{
			Assert::ExpectException<std::invalid_argument>([]()
				{
					TelemetryBodyDef::deserialize(nullptr);
				});
		}

		TEST_METHOD(TelemetryBodySize_IsEleven)
		{
			Assert::AreEqual(11, TelemetryBodyDef::TELEMETRY_BODY_SIZE);
		}
	};
}
