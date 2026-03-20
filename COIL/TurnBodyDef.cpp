#include <iostream>
#include <bitset>


class TurnBodyDef
{
public:
    uint8_t Direction : 8;
    uint8_t Duration : 8;
    uint8_t Power : 8;

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
    this->Power = 0;
}

char* TurnBodyDef::serialize() {
    // 8 for direction + 8 for duration + 8 for power
    char* buffer = new char[sizeof(TurnBodyDef)];
    std::memcpy(buffer, this, sizeof(TurnBodyDef));
    return buffer;
}

TurnBodyDef* TurnBodyDef::deserialize(char* data) {
    TurnBodyDef* returnValue = new TurnBodyDef();

    // because we know data order, and that this struct is packed we can just dump it into our class memory
    std:memcpy(returnValue, data, sizeof(TurnBodyDef));

    return returnValue;
}



// ai generated dev helper function
void TurnBodyDef::display() {
    std::cout << "--- Header Data ---" << std::endl;

    std::cout << "Direction: " << (int)Direction << " [Binary: " << std::bitset<8>(Direction) << "]" << std::endl;
    std::cout << "Duration: " << (int)Duration << " [Binary: " << std::bitset<8>(Duration) << "]" << std::endl;
    std::cout << "Power: " << (int)Power << " [Binary: " << std::bitset<8>(Direction) << "]" << std::endl;

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
