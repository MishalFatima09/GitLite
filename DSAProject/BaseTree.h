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
    virtual void insert(const string& key, const string& dir) = 0;
    virtual void print() = 0;
    virtual ~ColBasedTree() = default;
};
