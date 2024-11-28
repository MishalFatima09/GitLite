#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>

using namespace std;

// Instructor Hash Class
class InstructorHash {
public:
    // Compute Instructor Hash for an integer
    int computeHash(int number) {
        int hashValue = 1;
        while (number > 0) {
            int digit = number % 10; // Extract the last digit
            hashValue = (hashValue * digit) % 29; // Multiply and take modulo 29
            number /= 10; // Remove the last digit
        }
        return hashValue;
    }

    // Compute Instructor Hash for a string
    int computeHash(const string& str)
    {

        int hashValue = 1; // Start with an initial hash value of 1
        for (size_t i = 0; i < str.length(); i++) {
            int asciiValue = static_cast<int>(str[i]); // Get the ASCII value of the character at index i
            hashValue = (hashValue * asciiValue) % 29; // Multiply and take modulo 29
        }
        return hashValue;
    }
};