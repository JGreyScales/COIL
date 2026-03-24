#include "pch.h"
#include "CppUnitTest.h"
#include <bitset>


#include "PktDef.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace COILTEST
{
	TEST_CLASS(TurnBodyTests) {
		TEST_METHOD(DeserializeNegative) {
			TurnBodyDef* turnBodyInstance = TurnBodyDef::deserialize(nullptr);

			Assert::AreEqual<uint8_t>(0, turnBodyInstance->Direction);
			Assert::AreEqual<uint16_t>(0, turnBodyInstance->Duration);

			delete turnBodyInstance;
		}

		TEST_METHOD(DeserializeSanityCheck) {
			TurnBodyDef* turnBodyInstance = new TurnBodyDef();
			turnBodyInstance->Duration = 502;
			turnBodyInstance->Direction = 2;

			char* serializedData = turnBodyInstance->serialize();
			TurnBodyDef* deserializedInstance = TurnBodyDef::deserialize(serializedData);
		
			Assert::AreEqual<uint16_t>(502, deserializedInstance->Duration);
			Assert::AreEqual<uint8_t>(2, deserializedInstance->Direction);

			delete turnBodyInstance;
			delete deserializedInstance;
		}

		TEST_METHOD(getSizeCheck) {
			TurnBodyDef* turnBodyInstance = new TurnBodyDef();

			Assert::AreEqual(3, turnBodyInstance->getSize());

			delete turnBodyInstance;
		}

		TEST_METHOD(SanityInstanceCreation) {
			TurnBodyDef* turnBodyInstance = new TurnBodyDef();

			Assert::AreEqual<uint8_t>(0, turnBodyInstance->Direction);
			Assert::AreEqual<uint16_t>(0, turnBodyInstance->Duration);

			delete turnBodyInstance;
		}

		TEST_METHOD(BinaryOrderCheck) {
			TurnBodyDef* turnBodyInstance = new TurnBodyDef();
			turnBodyInstance->Duration = 502;
			turnBodyInstance->Direction = 2;

			char* serializedData = turnBodyInstance->serialize();

			Assert::AreEqual<uint8_t>(2, (uint8_t)serializedData[0]);
			Assert::AreEqual<uint8_t>(1, (uint8_t)serializedData[1]);
			Assert::AreEqual<uint8_t>(246, (uint8_t)serializedData[2]);

			delete turnBodyInstance;
		}
	};

	TEST_CLASS(DriveBodyTests) {
		TEST_METHOD(SanityInstanceCreation) {
			DriveBodyDef* driveBodyInstance = new DriveBodyDef();

			Assert::AreEqual(0, (int)driveBodyInstance->Direction);
			Assert::AreEqual(0, (int)driveBodyInstance->Duration);
			Assert::AreEqual(0, (int)driveBodyInstance->Power);

			delete driveBodyInstance;
		}

		TEST_METHOD(deserializeSanity) {
			DriveBodyDef* driveBodyInstance = new DriveBodyDef();
			driveBodyInstance->Direction = 4;
			driveBodyInstance->Duration = 10;
			driveBodyInstance->Power = 90;
			
			char* serializedData = driveBodyInstance->serialize();
			DriveBodyDef* deserialized = DriveBodyDef::deserialize(serializedData);

			Assert::AreEqual(4, (int)driveBodyInstance->Direction);
			Assert::AreEqual(10, (int)driveBodyInstance->Duration);
			Assert::AreEqual(90, (int)driveBodyInstance->Power);

			delete driveBodyInstance;
			delete deserialized;
		}

		TEST_METHOD(deserializeNegative) {
			DriveBodyDef* nulledDeserialize = DriveBodyDef::deserialize(nullptr);

			Assert::AreEqual(0, (int)nulledDeserialize->Direction);
			Assert::AreEqual(0, (int)nulledDeserialize->Duration);
			Assert::AreEqual(0, (int)nulledDeserialize->Power);

			delete nulledDeserialize;
		}

		TEST_METHOD(DriveBodyBinaryOrder) {
			DriveBodyDef* driveBodyInstance = new DriveBodyDef();
			driveBodyInstance->Direction = 4;
			driveBodyInstance->Duration = 10;
			driveBodyInstance->Power = 90;

			char* serializedData = driveBodyInstance->serialize();

			Assert::AreEqual(4, (int)serializedData[0]);
			Assert::AreEqual(10, (int)serializedData[1]);
			Assert::AreEqual(90, (int)serializedData[2]);

			delete driveBodyInstance;
		}

		TEST_METHOD(DriveBodyLength) {
			DriveBodyDef* driveBodyInstance = new DriveBodyDef();
			
			Assert::AreEqual(3, driveBodyInstance->getSize());

			delete driveBodyInstance;
		}
	};

	TEST_CLASS(AckBodyTests) {
		TEST_METHOD(sanityInstanceCreation) {
			AckBodyDef* ackInstance = new AckBodyDef();
			
			Assert::IsNull(ackInstance->body);
			delete ackInstance;
		}

		TEST_METHOD(getSizeSanityTest) {
			AckBodyDef* ackInstance = new AckBodyDef();
			ackInstance->setBody("someString");

			Assert::AreEqual(11, ackInstance->getSize());
			delete ackInstance;
		}

		TEST_METHOD(getSizeNegativeTest) {
			AckBodyDef* ackInstance = new AckBodyDef();

			Assert::AreEqual(0, ackInstance->getSize());
			delete ackInstance;
		}

		TEST_METHOD(setBodySanityCheck) {
			AckBodyDef* ackInstance = new AckBodyDef();
			ackInstance->setBody("Xx");

			Assert::AreEqual(0, strcmp("Xx", ackInstance->body));

			delete ackInstance;
		}

		TEST_METHOD(setBodyEmptyString) {
			AckBodyDef* ackInstance = new AckBodyDef();
			ackInstance->setBody(nullptr);

			Assert::IsNull(ackInstance->body);

			delete ackInstance;
		}

		TEST_METHOD(deserializeSanityCheck) {
			AckBodyDef* ackInstance = new AckBodyDef();
			ackInstance->setBody("someString");

			char* serializedData = ackInstance->serialize();
			AckBodyDef* deserializedData = AckBodyDef::deserialize(serializedData);

			Assert::AreEqual(0, strcmp("someString", deserializedData->body));
			delete ackInstance;
			delete deserializedData;
		}

		TEST_METHOD(deserializeNegative) {
			AckBodyDef* nulledDeserialized = AckBodyDef::deserialize(nullptr);

			Assert::IsNull(nulledDeserialized->body);

			delete nulledDeserialized;
		}
	};

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
			HeaderDef* HeaderInstance = new HeaderDef();
			Assert::AreEqual(3, HeaderInstance->getSize());
			delete HeaderInstance;
		}

		TEST_METHOD(HeaderBinaryOrder) {
			HeaderDef* HeaderInstance = new HeaderDef();

			HeaderInstance->PktCount = 1;
			HeaderInstance->Sleep = true;
			HeaderInstance->Drive = true;
			HeaderInstance->Length = 10;

			char* value = HeaderInstance->serialize();
			
			Assert::AreEqual<uint8_t>(1, (uint8_t)value[0]);
			Assert::AreEqual<uint8_t>(5, (uint8_t)value[1]);
			Assert::AreEqual<uint8_t>(10, (uint8_t)value[2]);
		}

		TEST_METHOD(deserializeNegative) {
			HeaderDef* nulledDeserialized = HeaderDef::deserialize(nullptr);

			Assert::AreEqual(0, (int)nulledDeserialized->PktCount);
			Assert::IsFalse(nulledDeserialized->Drive);
			Assert::IsFalse(nulledDeserialized->Status);
			Assert::IsFalse(nulledDeserialized->Sleep);
			Assert::IsFalse(nulledDeserialized->Ack);
			Assert::AreEqual(0, (int)nulledDeserialized->Padding);
			Assert::AreEqual(0, (int)nulledDeserialized->Length);

			delete nulledDeserialized;
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

		TEST_METHOD(TelemetryBodySize_IsEleven)
		{
			TelemetryBodyDef* TeleInstance = new TelemetryBodyDef();
			Assert::AreEqual(11, TeleInstance->getSize());
			delete TeleInstance;
		}

		TEST_METHOD(deserializeNegative) {
			TelemetryBodyDef* nulledDeserialized = TelemetryBodyDef::deserialize(nullptr);

			Assert::AreEqual<unsigned short int>(0, nulledDeserialized->HitCount);
			Assert::AreEqual<unsigned short int>(0, nulledDeserialized->Heading);
			Assert::AreEqual<unsigned short int>(0, nulledDeserialized->LastCmd);
			Assert::AreEqual<unsigned short int>(0, nulledDeserialized->LastCmdValue);
			Assert::AreEqual<unsigned short int>(0, nulledDeserialized->LastCmdPower);

			delete nulledDeserialized;
		}
	};
}
