#pragma once
#include<iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <filesystem> // For directory operations
using namespace std;


class InstructorHash {
public:
    int computeHash(const string& data) {
        int hashValue = 1;
        for (char c : data) {
            hashValue = (hashValue * static_cast<int>(c)) % 29;
        }
        return hashValue;
    }

    int computeHash(int number) {
        int hashValue = 1;
        while (number > 0) {
            int digit = number % 10;
            hashValue = (hashValue * digit) % 29;
            number /= 10;
        }
        return hashValue;
    }
};