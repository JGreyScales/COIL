#include "pch.h"
#include "CppUnitTest.h"


#include "Robot.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SOCKET_TESTS

// cant really test a whole bunch without mocking, I dont wanna do all that
{
	TEST_CLASS(ROBOTTESTS) {
		TEST_METHOD(sanityCheck) {
			Robot* robotInstance = new Robot(1);

			Assert::IsNull(robotInstance->socket);
			Assert::AreEqual<int>(1, robotInstance->id);

			delete robotInstance;
		}
	};

	TEST_CLASS(SocketTests) {
		TEST_METHOD(sanityCheck) {
			MySocket* socketInstance = new MySocket(MySocket::SERVER, "192.168.0.1", 1500, MySocket::TCP, 1000);

			Assert::AreEqual(0, socketInstance->GetIPAddr().compare("192.168.0.1"));
			Assert::AreEqual(1500, socketInstance->GetPort());
			Assert::AreEqual((int)MySocket::SERVER, (int)socketInstance->GetType());

		}
	};
}
