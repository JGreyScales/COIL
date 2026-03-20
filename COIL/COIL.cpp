// COIL.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "HeaderDef.h"
#include "TurnBodyDef.h"
#include "DriveBodyDef.h"

int main()
{

    std::cout << "SPECIFIC VALUE IS BIG ENDIAN" << "RAW BYTES IS LITTLE ENDIAN FOR MULTI-BYTE DATA" << std::endl;
    unsigned char rawBytes[] = { 0x00, 0x01, 0x03 };

    char* buffer = reinterpret_cast<char*>(rawBytes);


    DriveBodyDef* DriveBody = new DriveBodyDef();

    DriveBody->display();

    delete DriveBody;



    HeaderDef* header = new HeaderDef();

    header->display();

    delete header;

    HeaderDef* myHeader = HeaderDef::deserialize(buffer);

    std::cout << "\n\n\ndeserialized data" << std::endl;
    myHeader->display();

    delete myHeader;

    std::cout << "\n\n\n\n\n" << std::endl;

    TurnBodyDef* turnbody = new TurnBodyDef();

    turnbody->display();

    delete turnbody;

    TurnBodyDef* myTurnBody = TurnBodyDef::deserialize(buffer);

    std::cout << "\n\n\ndeserialized data" << std::endl;
    myTurnBody->display();

    delete myTurnBody;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
