#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <functional>
#include "BaseTree.h"
#include "BtreeNode.h" 
using namespace std;

class Btree : public ColBasedTree {
public:
    Node* root;
    int t; // Minimum degree

    Btree(int t) {
        this->root = nullptr;
        this->t = t;
    }

    void insert(const string& key, const string& dir) override {
        // Hash the key to an integer
        int value = hash<string>{}(key);

        if (root == nullptr) {
            root = new Node(t, true);
            root->keys[0] = value;
            root->count = 1;
        }
        else {
            if (root->count == 2 * t - 1) {
                Node* newNode = new Node(t, false);
                newNode->child[0] = root;
                newNode->splitting(0, root);

                int i = 0;
                if (newNode->keys[0] < value)
                    i++;
                newNode->child[i]->insertNonFull(value);

                root = newNode; // Update root
            }
            else {
                root->insertNonFull(value);
            }
        }

        // Save the node data to the directory
        string filePath = dir + "/" + key + ".txt";
        ofstream outFile(filePath);
        if (outFile.is_open()) {
            outFile << "Key: " << key << "\n";
            outFile << "Value: " << value << "\n";
            outFile.close();
            cout << "Node saved to file: " << filePath << endl;
        }
        else {
            cerr << "Error writing to file: " << filePath << endl;
        }
    }

    void print() override {
        if (root != nullptr) {
            cout << "B-tree traversal:" << endl;
            root->traverse();
            cout << endl;
        }
        else {
            cout << "The tree is empty!" << endl;
        }
    }

    ~Btree() {
        delete root; // Ensure proper cleanup of the root node and its children
    }
};
