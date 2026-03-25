#pragma once
#include <cstdint>
#include <limits>
#include <iostream>

namespace BitUtils {
    /**
     * Reverses the bits in an 8-bit byte.
     * Transforms 00001010 (10) into 01010000 (80)
     */
    inline uint8_t ReverseByte(uint8_t b) {
        b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
        b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
        b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
        return b;
    }
}

namespace ConsoleUtils {
    inline int getUserInt(int lower, int max) {
        int input;

        while (true) {
            std::cout << "Type a number between " << lower << " and " << max << ": ";

            if (std::cin >> input) {
                if (input >= lower && input <= max) {
                    return input;
                }
                std::cout << "Out of range. ";
            }
            else {
                std::cout << "That's not a number! ";
                std::cin.clear();
                std::cin.ignore(9999999999, '\n');
            }

            std::cout << "Please try again.\n" << std::endl;
        }
    }

    inline std::string getUserString() {
        std::string input = "";
        std::cin >> input;

        return input;
    }
}