#pragma once
#include <cstdint>
#include <limits>
#include <iostream>
#include <iomanip>

// this file is almost fully AI generated helper functions
// expect slop within here

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

    inline void displayChar(int size, char* data) {
        if (size <= 0 || data == nullptr) {
            std::cout << "Buffer is empty or null." << std::endl;
            return;
        }

        // Set formatting for the entire sequence
        std::cout << std::hex << std::uppercase;

        for (int i = 0; i < size; ++i) {
            // Cast to unsigned char to avoid sign extension (e.g., 0xFF appearing as 0xFFFFFFFF)
            unsigned char byte = static_cast<unsigned char>(data[i]);

            // Print hex value with leading zero if needed
            std::cout << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";

            // Standard 8 or 16 byte wrap for readability
            if ((i + 1) % 8 == 0) {
                std::cout << "\n";
            }
        }

        // Reset stream to decimal and lowercase to avoid side effects elsewhere
        std::cout << std::dec << std::nouppercase << std::endl;
    }

    inline void displayBin (int size, char* data) {
        if (size <= 0 || data == nullptr) {
            std::cout << "Buffer is empty or null." << std::endl;
            return;
        }

        for (int i = 0; i < size; ++i) {
            // Cast to unsigned to handle values 128-255 correctly
            unsigned char byte = static_cast<unsigned char>(data[i]);

            for (int j = 7; j >= 0; --j) {
                std::cout << (char)((byte >> j) & 1);
            }

            // Add a space between bytes for readability
            std::cout << " ";

            // Optional: Add a newline every 8 bytes to keep the console clean
            if ((i + 1) % 8 == 0) {
                std::cout << "\n";
            }
        }
        std::cout << std::endl;
    }
}

// this part is like half human half ai :)
namespace ConsoleUtils {
    inline int getUserInt(int lower, int max) {
        int input;

        while (true) {
            std::cout << "Type a number between " << lower << " and " << max << ": ";

            if (std::cin >> input) {
                if (input >= lower && input <= max) {
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    return input;
                }
                std::cout << "Out of range. ";
            }
            else {
                std::cout << "That's not a number! ";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }

            std::cout << "Please try again.\n" << std::endl;
        }
    }

    inline std::string getUserString() {
        std::string input;
        if (!(std::cin >> input)) {
            std::cin.clear();
        }

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return input;
    }

    inline bool validateRobotId(int robotId)
    {
        if (robotId < 0 || robotId > 1)
        {
			return false;
        }
		return true;
    }

    inline bool validateDirection(int direction)
    {
        if (direction < 1 || direction > 4)
        {
			return false;
        }
		return true;
    }

    inline bool validateDuration(int duration)
    {
        if (duration < 1 || duration > 100)
        {
			return false;
        }
		return true;
    }

    inline bool validatePower(int power)
    {
        if (power < 80 || power > 100)
        {
			return false;
        }
		return true;
    }
}