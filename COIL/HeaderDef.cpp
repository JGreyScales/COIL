#include <iostream>

class HeaderDef
{
public:
    int PktCount;
    bool Drive;
    bool Status;
    bool Sleep;
    bool Ack;
    char* Padding;
    int Length;

    HeaderDef();

    char* serialize();
    HeaderDef static deserialize(char* data);
    void display();

    ~HeaderDef();
};

HeaderDef::HeaderDef()
{
    this->PktCount = 0;
    this->Drive = false;
    this->Status = false;
    this->Sleep = false;
    this->Ack = false;
    this->Padding = "";
    this->Length = 0;
}

char* HeaderDef::serialize(){

}

HeaderDef HeaderDef::deserialize(char* data){

}


void HeaderDef::display(){
    std::cout << "\nPacket count: " << PktCount <<
    "\nDrive Status: " << Drive << "\nStatus: " << Status << 
    "\nSleep: " << Sleep << "\nAck: " << Ack << 
    "\nPadding (garbage): " << Padding << "\nLength: " << Length << std::endl;

    return;
}


HeaderDef::~HeaderDef()
{
}
