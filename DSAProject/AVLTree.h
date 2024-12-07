#pragma once
#include<iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem> // For directory operations
#include <unordered_map> // For indexing
#include "BaseTree.h"

using namespace std;

struct AVLNode {
    string key;        // Key for this node
    AVLNode* left;     // Pointer to the left child
    AVLNode* right;    // Pointer to the right child
    int height;        // Height of the node
    bool isLazy;       // Lazy flag to indicate if the node file has been created

    AVLNode(string k) : key(k), left(nullptr), right(nullptr), height(1), isLazy(true) {}
};

class MerkleAVLTree : public ColBasedTree {
private:
    AVLNode* root;
    unordered_map<string, string> index;  // Index to map keys to file paths

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

        // Lazily create node files
        saveNodeIfLazy(y, dir);
        saveNodeIfLazy(x, dir);

        return x;
    }

    AVLNode* leftRotate(AVLNode* x, const string& dir) {
        AVLNode* y = x->right;
        AVLNode* T2 = y->left;

        y->left = x;
        x->right = T2;

        updateHeight(x);
        updateHeight(y);

        // Lazily create node files
        saveNodeIfLazy(x, dir);
        saveNodeIfLazy(y, dir);

        return y;
    }

    AVLNode* insertNode(AVLNode* node, const string& key, const string& dir) {
        if (node == nullptr) {
            AVLNode* newNode = new AVLNode(key);
            // Add node file path to the index (mapping key to file path)
            index[key] = dir + "/" + key + ".txt";
            return newNode;
        }

        if (key < node->key) {
            node->left = insertNode(node->left, key, dir);
        }
        else if (key > node->key) {
            node->right = insertNode(node->right, key, dir);
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

        return node;
    }

    // Lazy file creation
    void saveNodeIfLazy(AVLNode* node, const string& dir) {
        if (node == nullptr) return;

        if (node->isLazy) {
            string fileName = dir + "/" + node->key + ".txt";
            ofstream file(fileName);
            if (file) {
                file << "Key: " << node->key << endl;
                file << "Left: " << (node->left ? node->left->key : "NULL") << endl;
                file << "Right: " << (node->right ? node->right->key : "NULL") << endl;
                file.close();

                node->isLazy = false;  // Mark the node as having its file created
                cout << "Node file created lazily: " << fileName << endl;
            }
        }
    }

    // Efficient search using the unordered_map index
    bool searchNode(const string& key, string& filePath) {
        auto it = index.find(key);  // Search in the index
        if (it != index.end()) {
            filePath = it->second; // Retrieve the file path if found
            return true;
        }
        return false; // Key not found
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

        // Make sure to create the files lazily when printing
        saveNodeIfLazy(node, "");
    }

public:
    MerkleAVLTree() : root(nullptr) {}

    void insert(const string& key, const string& dir) override {
        root = insertNode(root, key, dir);
    }

    bool search(const string& key, string& filePath) {
        return searchNode(key, filePath);  // Try to find the key in the index
    }

    void print() override {
        printTree(root, "");
    }
};
