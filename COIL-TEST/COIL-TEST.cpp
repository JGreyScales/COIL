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

			uint16_t pktCount = 10;
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
			Assert::AreEqual(4, HeaderInstance->getSize());
			delete HeaderInstance;
		}

		TEST_METHOD(HeaderBinaryOrder) {
			HeaderDef* HeaderInstance = new HeaderDef();

			HeaderInstance->PktCount = 1;
			HeaderInstance->Sleep = true;
			HeaderInstance->Drive = true;
			HeaderInstance->Length = 10;

			char* value = HeaderInstance->serialize();
			
			Assert::AreEqual<uint8_t>(0, (uint8_t)value[0]);
			Assert::AreEqual<uint8_t>(1, (uint8_t)value[1]);
			Assert::AreEqual<uint8_t>(160, (uint8_t)value[2]);
			Assert::AreEqual<uint8_t>(10, (uint8_t)value[3]);
		}

		TEST_METHOD(deserializeNegative) {
			HeaderDef* nulledDeserialized = HeaderDef::deserialize(nullptr);

			Assert::AreEqual<uint16_t>(0, nulledDeserialized->PktCount);
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

	TEST_CLASS(PktDefTests) {
		TEST_METHOD(validate_Direction_enum) {
			Assert::AreEqual(1, (int)PktDef::FORWARD);
			Assert::AreEqual(2, (int)PktDef::BACKWARD);
			Assert::AreEqual(3, (int)PktDef::RIGHT);
			Assert::AreEqual(4, (int)PktDef::LEFT);
		}

		TEST_METHOD(validate_CRC_and_HeaderSize) {
			Assert::AreEqual(4, PktDef::HEADERSIZE);
			Assert::AreEqual(1, PktDef::CRCSIZE);
		}

		TEST_METHOD(countSetBits) {

			PktDef* pktInstance = new PktDef();

			// written letter is the amount of 1's in the byte to represent
			// the number
			uint8_t eight = 255;
			uint8_t four = 147;
			uint8_t two = 3;

			uint8_t numbers[3] = { eight, four, two };

			for (size_t i = 0; i < sizeof(numbers); i++)
			{
				// floor divide with type casting
				int expected = (int)(8 / (1 + i));
				int result = pktInstance->countSetBits((char*)&numbers[i], 1);
				Assert::AreEqual(expected, result);
			}

			delete pktInstance;
		}

		TEST_METHOD(countSetBitsNegative) {
			PktDef* pktInstance = new PktDef();

			Assert::AreEqual<unsigned char>(0, pktInstance->countSetBits(nullptr, 99999));

			delete pktInstance;
		}

		TEST_METHOD(serializeFromRawData) {
			PktDef* pktInstance = new PktDef();

			TelemetryBodyDef* teleInstance = new TelemetryBodyDef();
			teleInstance->Heading = 3;
			teleInstance->LastCmdPower = 5;

			pktInstance->SetCmd(PktDef::RESPONSE);
			pktInstance->SetBodyData(teleInstance);

			char* serialData = pktInstance->GenPacket();

			PktDef* deserializedInstance = new PktDef(serialData);
			deserializedInstance->SetPktCount(0);

			char* deserializedSerializedData = deserializedInstance->GenPacket();

			for (int i = 0; i < pktInstance->GetLength(); ++i) {
				std::wstring msg = L"Error at index " + std::to_wstring(i);
				Assert::AreEqual((int)serialData[i], (int)deserializedSerializedData[i], msg.c_str());
			}

			delete pktInstance;
			delete deserializedInstance;
		}

		TEST_METHOD(serializeNonResponseTypeGivesNullPtr) {
			PktDef* pktInstance = new PktDef();
			pktInstance->SetCmd(PktDef::DRIVE);

			char* serial = pktInstance->GenPacket();

			PktDef* deserialized = new PktDef(serial);

			Assert::IsNull(deserialized->GetBodyData());

			delete deserialized;
			delete pktInstance;
		}

		TEST_METHOD(setCommand) {
			PktDef* pktInstance = new PktDef();

			Assert::AreEqual((int)PktDef::UNDEFINED, (int)pktInstance->GetCmd());

			pktInstance->SetCmd(PktDef::DRIVE);

			Assert::AreEqual((int)PktDef::DRIVE, (int)pktInstance->GetCmd());
			Assert::IsTrue(pktInstance->getPacket().Header->Drive);
			delete pktInstance;
		}

		TEST_METHOD(setBodyData) {
			DriveBodyDef* driveBodyInstance = new DriveBodyDef();

			PktDef* pktInstance = new PktDef();
			pktInstance->SetBodyData(driveBodyInstance);

			// memory address in the pkt is the same as the drive instance
			Assert::IsTrue(driveBodyInstance == pktInstance->getPacket().body);
			// 6 for header size (4) + driveBodySize (3) + crcSize (1)
			Assert::AreEqual(8, pktInstance->GetLength());

			delete pktInstance;

		}

		TEST_METHOD(setBodyDataNegative) {
			PktDef* pktInstrance = new PktDef();
			pktInstrance->SetBodyData(nullptr);

			Assert::IsNull(pktInstrance->getPacket().body);

			delete pktInstrance;
		}

		TEST_METHOD(setPktCount) {
			PktDef* pktInstance = new PktDef();

			pktInstance->SetPktCount(50);

			Assert::AreEqual(50, pktInstance->GetPktCount());
			delete pktInstance;
		}

		TEST_METHOD(setPktCountNegative) {
			PktDef* pktInstance = new PktDef();

			pktInstance->SetPktCount(5);
			pktInstance->SetPktCount(-24);

			Assert::AreEqual(5, pktInstance->GetPktCount());

			delete pktInstance;
		}

		TEST_METHOD(getCmd) {
			PktDef* pktInstance = new PktDef();

			pktInstance->SetCmd(PktDef::SLEEP);

			Assert::AreEqual((int)PktDef::SLEEP, (int)pktInstance->GetCmd());

			delete pktInstance;
		}

		TEST_METHOD(getAck) {
			PktDef* pktInstance = new PktDef();

			pktInstance->getPacket().Header->Ack = 1;

			Assert::IsTrue(pktInstance->GetAck());
				
			delete pktInstance;
		}

		TEST_METHOD(getLength) {
			PktDef* pktInstance = new PktDef();

			pktInstance->getPacket().Header->Length = 50;

			Assert::AreEqual(50, pktInstance->GetLength());

			delete pktInstance;
		}

		TEST_METHOD(getBodyData) {
			PktDef* pktInstance = new PktDef();
			DriveBodyDef* driveInstance = new DriveBodyDef();
			driveInstance->Direction = 5;
			driveInstance->Duration = 4;
			pktInstance->SetBodyData(driveInstance);

			char* serialData = pktInstance->GetBodyData();
			Assert::AreEqual(5, (int)serialData[0]);
			Assert::AreEqual(4, (int)serialData[1]);
			Assert::AreEqual(0, (int)serialData[2]);


			delete pktInstance;
		}

		TEST_METHOD(getPktCount) {
			PktDef* pktInstance = new PktDef();
			
			Assert::AreEqual(0, pktInstance->GetPktCount());

			pktInstance->GenPacket();

			Assert::AreEqual(1, pktInstance->GetPktCount());

			delete pktInstance;
		}

		TEST_METHOD(checkCRC) {
			PktDef* pktInstance = new PktDef();

			char* data = pktInstance->GenPacket();
			
			// crc should be 0 since everything defaults to 0 for everything
			Assert::IsTrue(pktInstance->CheckCRC(data, 5));

			delete pktInstance;
		}

		TEST_METHOD(CheckCRCInvalidData) {
			PktDef* pktInstance = new PktDef();

			Assert::IsFalse(pktInstance->CheckCRC(nullptr, 50));

			delete pktInstance;
		}

		TEST_METHOD(CheckCRCInvalidSize){
			PktDef* pktInstance = new PktDef();

			Assert::IsFalse(pktInstance->CheckCRC("", 4));

			delete pktInstance;
		}

		TEST_METHOD(checkCRCInvalidCalculationVsRecieved) {
			PktDef* pktInstance = new PktDef();

			char* data = pktInstance->GenPacket();

			// no body, so byte 4 is the crc byte
			data[4] = 3;

			Assert::IsFalse(pktInstance->CheckCRC(data, 5));

			delete pktInstance;
		}

		TEST_METHOD(genPacketDrive) {
			PktDef* pktInstance = new PktDef();
			DriveBodyDef* driveBodyInstance = new DriveBodyDef();
			driveBodyInstance->Direction = 2;

			pktInstance->SetCmd(PktDef::DRIVE);
			pktInstance->SetBodyData(driveBodyInstance);


			// 8 bits
			// 4 header
			// 3 body
			// 1 crc
			char* data = pktInstance->GenPacket();

			// header
			Assert::AreEqual<uint8_t>(0, data[0]);
			Assert::AreEqual<uint8_t>(0, data[1]);
			Assert::AreEqual<uint8_t>(128, data[2]);
			Assert::AreEqual<uint8_t>(8, data[3]);

			// body
			Assert::AreEqual<uint8_t>(2, data[4]);
			Assert::AreEqual<uint8_t>(0, data[5]);
			Assert::AreEqual<uint8_t>(0, data[6]);

			// crc
			Assert::AreEqual<uint8_t>(3, data[7]);

			delete pktInstance;
		}

		TEST_METHOD(genPacketTurn) {
			PktDef* pktInstance = new PktDef();
			TurnBodyDef* driveBodyInstance = new TurnBodyDef();
			driveBodyInstance->Duration = 5;

			pktInstance->SetCmd(PktDef::DRIVE);
			pktInstance->SetBodyData(driveBodyInstance);


			// 8 bits
			// 4 header
			// 3 body
			// 1 crc
			char* data = pktInstance->GenPacket();
			char byte5 = data[4];
			char byte6 = data[5];
			char byte7 = data[6];

			// header
			Assert::AreEqual<uint8_t>(0, data[0]);
			Assert::AreEqual<uint8_t>(0, data[1]);
			Assert::AreEqual<uint8_t>(128, data[2]);
			Assert::AreEqual<uint8_t>(8, data[3]);

			// body
			Assert::AreEqual<uint8_t>(0, data[4]);
			Assert::AreEqual<uint8_t>(0, data[5]);
			Assert::AreEqual<uint8_t>(5, data[6]);

			// crc
			Assert::AreEqual<uint8_t>(4, data[7]);

			delete pktInstance;
		}

		TEST_METHOD(genPacketSleep) {
			PktDef* pktInstance = new PktDef();

			pktInstance->SetCmd(PktDef::SLEEP);


			// 5 bits
			// 4 header
			// 0 body
			// 1 crc
			char* data = pktInstance->GenPacket();

			// header
			Assert::AreEqual<uint8_t>(0, data[0]);
			Assert::AreEqual<uint8_t>(0, data[1]);
			Assert::AreEqual<uint8_t>(32, data[2]);
			Assert::AreEqual<uint8_t>(5, data[3]);

			// crc
			Assert::AreEqual<uint8_t>(3, data[4]);

			delete pktInstance;
		}

		TEST_METHOD(reset) {
			PktDef* pktInstance = new PktDef();

			pktInstance->SetCmd(PktDef::SLEEP);

			pktInstance->Reset();
			Assert::AreEqual((int)PktDef::UNDEFINED, (int)pktInstance->GetCmd());

			delete pktInstance;
		}
	};
}
