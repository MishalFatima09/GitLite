#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include "BaseTree.h"

using namespace std;

enum Color { RED, BLACK };

// Red-Black Tree Node Structure
struct RBNode {
    string key;
    Color color; // RED or BLACK
    RBNode* left;
    RBNode* right;
    RBNode* parent;

    RBNode(string k, Color c) : key(k), color(c), left(nullptr), right(nullptr), parent(nullptr) {}
};

class RBTree : public ColBasedTree {
private:
    RBNode* root;
    RBNode* TNULL; // Sentinel node for null references

    // Comprehensive file saving method
    void saveTreeToFiles(const string& dir) {
        // Ensure the directory exists
        if (!std::filesystem::exists(dir)) {
            std::filesystem::create_directory(dir);
        }

        // Clear any existing files in the directory
        for (const auto& entry : std::filesystem::directory_iterator(dir)) {
            std::filesystem::remove(entry.path());
        }

        // Save the entire tree structure recursively
        saveNodeAndChildren(root, dir);
    }

    // Recursive method to save each node and its children
    void saveNodeAndChildren(RBNode* node, const string& dir) {
        if (node == nullptr || node == TNULL) return;

        // Create file for current node
        string fileName = dir + "/" + node->key + ".txt";
        ofstream file(fileName);

        if (file.is_open()) {
            // Node details
            file << "Key: " << node->key << endl;
            file << "Color: " << (node->color == RED ? "RED" : "BLACK") << endl;

            // Left child information
            if (node->left != nullptr && node->left != TNULL) {
                file << "Left: " << node->left->key << endl;
            }
            else {
                file << "Left: NULL" << endl;
            }

            // Right child information
            if (node->right != nullptr && node->right != TNULL) {
                file << "Right: " << node->right->key << endl;
            }
            else {
                file << "Right: NULL" << endl;
            }

            // Parent information
            if (node->parent != nullptr && node->parent != TNULL) {
                file << "Parent: " << node->parent->key << endl;
            }
            else {
                file << "Parent: NULL" << endl;
            }

            file.close();
        }

        // Recursively save children
        saveNodeAndChildren(node->left, dir);
        saveNodeAndChildren(node->right, dir);
    }

    // Initialize the TNULL node
    void initializeTNULL() {
        TNULL = new RBNode("", BLACK);
        TNULL->left = TNULL;
        TNULL->right = TNULL;
        TNULL->parent = nullptr;
    }

    // Left rotation
    void leftRotate(RBNode* x, const string& dir) {
        if (x == nullptr) return;
        RBNode* y = x->right;
        x->right = y->left;
        if (y->left != TNULL) {
            y->left->parent = x;
        }
        y->parent = x->parent;
        if (x->parent == nullptr) {
            root = y;
        }
        else if (x == x->parent->left) {
            x->parent->left = y;
        }
        else {
            x->parent->right = y;
        }
        y->left = x;
        x->parent = y;

        // Save updated tree structure after rotation
        saveTreeToFiles(dir);
    }

    // Right rotation
    void rightRotate(RBNode* x, const string& dir) {
        if (x == nullptr) return;
        RBNode* y = x->left;
        x->left = y->right;
        if (y->right != TNULL) {
            y->right->parent = x;
        }
        y->parent = x->parent;
        if (x->parent == nullptr) {
            root = y;
        }
        else if (x == x->parent->right) {
            x->parent->right = y;
        }
        else {
            x->parent->left = y;
        }
        y->right = x;
        x->parent = y;

        // Save updated tree structure after rotation
        saveTreeToFiles(dir);
    }

    // Fix any violations of Red-Black tree properties after insertion
    void insertFix(RBNode* node, const string& dir) {
        while (node->parent != nullptr && node->parent->color == RED) {
            if (node->parent == node->parent->parent->left) {
                RBNode* uncle = node->parent->parent->right;
                if (uncle->color == RED) {
                    node->parent->color = BLACK;
                    uncle->color = BLACK;
                    node->parent->parent->color = RED;
                    node = node->parent->parent;
                }
                else {
                    if (node == node->parent->right) {
                        node = node->parent;
                        leftRotate(node, dir);
                    }
                    node->parent->color = BLACK;
                    node->parent->parent->color = RED;
                    rightRotate(node->parent->parent, dir);
                }
            }
            else {
                RBNode* uncle = node->parent->parent->left;
                if (uncle->color == RED) {
                    node->parent->color = BLACK;
                    uncle->color = BLACK;
                    node->parent->parent->color = RED;
                    node = node->parent->parent;
                }
                else {
                    if (node == node->parent->left) {
                        node = node->parent;
                        rightRotate(node, dir);
                    }
                    node->parent->color = BLACK;
                    node->parent->parent->color = RED;
                    leftRotate(node->parent->parent, dir);
                }
            }

            // Prevent infinite loop
            if (node == root) break;
        }
        root->color = BLACK;

        // Save the entire tree structure after fixing
        saveTreeToFiles(dir);
    }

    // Print the tree structure
    void printTree(RBNode* node, string indent = "") {
        if (node == TNULL) return;

        cout << indent << "Node Key: " << node->key
            << ", Color: " << (node->color == RED ? "RED" : "BLACK") << endl;

        if (node->left != TNULL) {
            cout << indent << "Left: " << endl;
            printTree(node->left, indent + "  ");
        }
        if (node->right != TNULL) {
            cout << indent << "Right: " << endl;
            printTree(node->right, indent + "  ");
        }
    }

public:
    RBTree() {
        initializeTNULL();
        root = TNULL;
    }

    // Insert a new key into the Red-Black Tree
    void insert(const string& key, const string& dir) override {
        RBNode* node = new RBNode(key, RED);
        node->parent = nullptr;
        node->left = TNULL;
        node->right = TNULL;

        RBNode* y = nullptr;
        RBNode* x = root;

        while (x != TNULL) {
            y = x;
            if (node->key < x->key) {
                x = x->left;
            }
            else {
                x = x->right;
            }
        }

        node->parent = y;
        if (y == nullptr) {
            root = node;
        }
        else if (node->key < y->key) {
            y->left = node;
        }
        else {
            y->right = node;
        }

        // Always save the entire tree structure after insertion
        saveTreeToFiles(dir);

        if (node->parent == nullptr) {
            node->color = BLACK;
            return;
        }

        if (node->parent->parent == nullptr) {
            return;
        }

        insertFix(node, dir);
    }

    // Print the tree
    void print() override {
        printTree(root, "");
    }
};