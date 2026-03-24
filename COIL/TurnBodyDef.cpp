#include <iostream>
#include <bitset>

#pragma pack(push, 1)
class TurnBodyDef
{
public:
    uint8_t Direction : 8;
    uint16_t Duration : 16;

    TurnBodyDef();

    char* serialize();
    static TurnBodyDef* deserialize(char* data);
    void display();

    ~TurnBodyDef();
};

TurnBodyDef::TurnBodyDef()
{
    this->Direction = 0;
    this->Duration = 0;
}

char* TurnBodyDef::serialize() {
    char* buffer = new char[sizeof(TurnBodyDef)];
    std::memcpy(buffer, this, sizeof(TurnBodyDef));
    return buffer;
}

TurnBodyDef* TurnBodyDef::deserialize(char* data) {
    TurnBodyDef* returnValue = new TurnBodyDef();

    std::memcpy(returnValue, data, sizeof(TurnBodyDef));

    return returnValue;
}



// ai generated dev helper function
void TurnBodyDef::display() {
    std::cout << "--- TurnBody Data ---" << std::endl;

    std::cout << "Direction: " << (int)Direction << " [Binary: " << std::bitset<8>(Direction) << "]" << std::endl;
    std::cout << "Duration: " << (int)Duration << " [Binary: " << std::bitset<16>(Duration) << "]" << std::endl;

    uint8_t* rawData = reinterpret_cast<uint8_t*>(this);

    std::cout << "\n--- Raw Memory Layout (Bytes) ---" << std::endl;
    for (size_t i = 0; i < sizeof(TurnBodyDef); ++i) {
        std::cout << "Byte " << i << ": " << std::bitset<8>(rawData[i]) << std::endl;
    }   

    std::cout << "-------------------" << std::endl;
}


TurnBodyDef::~TurnBodyDef()
{
}
