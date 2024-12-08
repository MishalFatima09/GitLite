#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <filesystem> 
using namespace std;

class ColBasedTree {
public:
    // Updated insert method to use std::vector<std::string> instead of char**
    virtual void insert(const string& key, const vector<string>& row, int rowCount, const string& dir) = 0;
    virtual void update(const int columnIndex, const string& newValue,
        const int conditionIndex, const string& conditionValue, const string& dir) = 0;
    virtual void print() = 0;
    virtual ~ColBasedTree() = default;
};
