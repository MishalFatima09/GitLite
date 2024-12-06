#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include "BaseTree.h" 
#include "Hasher.h"   // For hashing logic

using namespace std;

enum Color { RED, BLACK };

// Red-Black Tree Node Structure
struct RBNode {
    string key;
    int hashValue;
    Color color; // RED or BLACK
    RBNode* left;
    RBNode* right;
    RBNode* parent;

    RBNode(string k, int h, Color c) : key(k), hashValue(h), color(c), left(nullptr), right(nullptr), parent(nullptr) {}
};

class RBTree : public ColBasedTree {
private:
    RBNode* root;
    RBNode* TNULL; // Sentinel node for null references
    InstructorHash hasher;

    void initializeTNULL() {
        TNULL = new RBNode("", 0, BLACK);
        TNULL->left = nullptr;
        TNULL->right = nullptr;
        TNULL->parent = nullptr;
    }

    void saveNodeToFile(RBNode* node, const string& dir) {
        if (node == TNULL) return;

        if (!std::filesystem::exists(dir)) {
            std::filesystem::create_directory(dir);
        }

        string fileName = dir + "/" + node->key + ".txt";
        ofstream file(fileName);
        if (file) {
            file << "Key: " << node->key << endl;
            file << "Hash: " << node->hashValue << endl;
            file << "Color: " << (node->color == RED ? "RED" : "BLACK") << endl;
            file << "Left: " << (node->left != TNULL ? node->left->key : "NULL") << endl;
            file << "Right: " << (node->right != TNULL ? node->right->key : "NULL") << endl;
            file.close();
            cout << "Node saved to file: " << fileName << endl;
        }
    }

    void leftRotate(RBNode* x, const string& dir) {
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

        saveNodeToFile(x, dir);
        saveNodeToFile(y, dir);
    }

    void rightRotate(RBNode* x, const string& dir) {
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

        saveNodeToFile(x, dir);
        saveNodeToFile(y, dir);
    }

    void insertFix(RBNode* node, const string& dir) {
        while (node->parent->color == RED) {
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
        }
        root->color = BLACK;
    }

    void printTree(RBNode* node, string indent = "") {
        if (node == TNULL) return;

        cout << indent << "Node Key: " << node->key << ", Hash: " << node->hashValue
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

    void insert(const string& key, const string& dir) override {
        RBNode* node = new RBNode(key, hasher.computeHash(key), RED);
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

        if (node->parent == nullptr) {
            node->color = BLACK;
            saveNodeToFile(node, dir);
            return;
        }

        if (node->parent->parent == nullptr) {
            saveNodeToFile(node, dir);
            return;
        }

        insertFix(node, dir);
        saveNodeToFile(node, dir);
    }

    int getRootHash() override {
        return root ? root->hashValue : 0;
    }

    void print() override {
        printTree(root, "");
    }
};
