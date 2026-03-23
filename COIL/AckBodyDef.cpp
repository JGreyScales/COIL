#include <iostream>
#include <bitset>
#include <cstring>

class AckBodyDef {
public:
    char* body;

    AckBodyDef();
    ~AckBodyDef();

    
    char* serialize();
    static AckBodyDef* deserialize(char* data);
    void display();

    
    void setBody(const char* newBody);
};

AckBodyDef::AckBodyDef() {
    this->body = nullptr;
}

AckBodyDef::~AckBodyDef() {
    delete[] body;
}

void AckBodyDef::setBody(const char* newBody) {
    delete[] body;
    if (newBody) {
        const int size = (int)std::strlen(newBody) + 1;
        body = new char[size];
        strcpy_s(body, size, newBody);
    }
    else {
        body = nullptr;
    }
}

char* AckBodyDef::serialize() {
    std::cout << "Not implimented for this class" << std::endl;
    return nullptr;
}

 AckBodyDef* AckBodyDef::deserialize(char* data) {
    AckBodyDef* returnValue = new AckBodyDef();
    if (data) {
        returnValue->setBody(data);
    }
    return returnValue;
}

void AckBodyDef::display() {
    std::cout << "--- AckBody Content ---" << std::endl;

    if (body) {
        std::cout << "String: " << body << std::endl;
        std::cout << "Address: " << static_cast<void*>(body) << std::endl;

        size_t len = std::strlen(body);
        std::cout << "\n--- Raw Memory Layout (Bytes) ---" << std::endl;
        for (size_t i = 0; i <= len; ++i) {
            std::cout << "Byte " << i << " ['" << (body[i] ? body[i] : '0') << "']: "
                << std::bitset<8>(static_cast<unsigned char>(body[i])) << std::endl;
        }
    }
    else {
        std::cout << "[Body is Empty/Null]" << std::endl;
    }

    std::cout << "-----------------------" << std::endl;
}