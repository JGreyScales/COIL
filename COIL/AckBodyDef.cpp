#include <iostream>
#include <bitset>
#include <cstring>

#include "AckBodyDef.h"

AckBodyDef::AckBodyDef() {
    this->body = nullptr;
}

AckBodyDef::~AckBodyDef() {
    if (this->body != nullptr) {
        delete[] body;
    }
}

void AckBodyDef::setBody(const char* newBody) {
    delete[] body;
    if (newBody) {
        const int size = (int)std::strlen(newBody);
        body = new char[1024];
        strcpy_s(body, size, newBody);
    }
    else {
        body = nullptr;
    }
}

int AckBodyDef::getSize() {
    if (this->body != nullptr) {
        // +1 to account for the terminator
        return 1 + static_cast<int>(strlen(this->body));
    }
    else {
        return 0;
    }
}

// ai generated
// this will literally never be used
// only useful for test cases
char* AckBodyDef::serialize() {
    if (this->body == nullptr) return nullptr;

    int size = getSize();
    char* buffer = new char[1024];
    strcpy_s(buffer, size, this->body);
    return buffer;
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