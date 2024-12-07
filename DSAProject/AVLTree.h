#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem> // For directory operations
#include "BaseTree.h"

using namespace std;

struct AVLNode {
    string key;                // Key for this node
    AVLNode* left;             // Pointer to the left child
    AVLNode* right;            // Pointer to the right child
    int height;                // Height of the node
    vector<string> row;        // Row data (vector of strings)
    int rowCount;              // Number of elements in the row

    // Constructor that initializes key and row data
    AVLNode(string k, const vector<string>& r = {}, int count = 0)
        : key(k), left(nullptr), right(nullptr), height(1), row(r), rowCount(count) {}
};

class MerkleAVLTree : public ColBasedTree {
private:
    AVLNode* root;

    int height(AVLNode* node) {
        return node ? node->height : 0;
    }

    void updateHeight(AVLNode* node) {
        node->height = 1 + max(height(node->left), height(node->right));
    }

    AVLNode* rightRotate(AVLNode* y, const string& dir) {
        AVLNode* x = y->left;
        AVLNode* T2 = x->right;

        x->right = y;
        y->left = T2;

        updateHeight(y);
        updateHeight(x);

        saveNodeToFile(y, dir);
        saveNodeToFile(x, dir);

        return x;
    }

    AVLNode* leftRotate(AVLNode* x, const string& dir) {
        AVLNode* y = x->right;
        AVLNode* T2 = y->left;

        y->left = x;
        x->right = T2;

        updateHeight(x);
        updateHeight(y);

        saveNodeToFile(x, dir);
        saveNodeToFile(y, dir);

        return y;
    }

    AVLNode* insertNode(AVLNode* node, const string& key, const vector<string>& row, int rowCount, const string& dir) {
        if (node == nullptr) {
            // Create a new node and store row data
            AVLNode* newNode = new AVLNode(key, row, rowCount);
            saveNodeToFile(newNode, dir);
            return newNode;
        }

        if (key < node->key) {
            node->left = insertNode(node->left, key, row, rowCount, dir);
        }
        else if (key > node->key) {
            node->right = insertNode(node->right, key, row, rowCount, dir);
        }
        else {
            return node; // No duplicates allowed
        }

        updateHeight(node);

        int balance = height(node->left) - height(node->right);

        if (balance > 1 && key < node->left->key) {
            return rightRotate(node, dir);
        }

        if (balance < -1 && key > node->right->key) {
            return leftRotate(node, dir);
        }

        if (balance > 1 && key > node->left->key) {
            node->left = leftRotate(node->left, dir);
            return rightRotate(node, dir);
        }

        if (balance < -1 && key < node->right->key) {
            node->right = rightRotate(node->right, dir);
            return leftRotate(node, dir);
        }

        saveNodeToFile(node, dir);
        return node;
    }

    void saveNodeToFile(AVLNode* node, const string& dir) {
        if (node == nullptr) return;

        if (!std::filesystem::exists(dir)) {
            std::filesystem::create_directory(dir);
        }

        string fileName = dir + "/" + node->key + ".txt";
        ofstream file(fileName);

        if (file) {
            file << "Key: " << node->key << endl;
            file << "Left: " << (node->left ? node->left->key : "NULL") << endl;
            file << "Right: " << (node->right ? node->right->key : "NULL") << endl;

            // Save the row data
            file << "Row Data: ";
            for (const auto& value : node->row) {
                file << value << " ";  // Each element in the row separated by a space
            }
            file << endl;

            file.close();
            cout << "Node and row data saved to file: " << fileName << endl;
        }
    }

    void printTree(AVLNode* node, string indent = "") {
        if (node == nullptr) return;

        cout << indent << "Node Key: " << node->key << endl;

        if (node->left != nullptr) {
            cout << indent << "Left: " << endl;
            printTree(node->left, indent + "  ");
        }
        if (node->right != nullptr) {
            cout << indent << "Right: " << endl;
            printTree(node->right, indent + "  ");
        }
    }

public:
    MerkleAVLTree() : root(nullptr) {}

    // Updated insert method to use vector<string> instead of char**
    void insert(const string& key, const vector<string>& row, int rowCount, const string& dir) override {
        root = insertNode(root, key, row, rowCount, dir);
    }

    void print() override {
        printTree(root, "");
    }
};
