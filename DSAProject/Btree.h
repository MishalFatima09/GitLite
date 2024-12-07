#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <filesystem>
#include "BaseTree.h"

using namespace std;

class BTreeNode {
public:
    vector<string> keys;             // Store actual keys (strings)
    vector<BTreeNode*> children;     // Pointers to child nodes
    bool isLeaf;                     // True if the node is a leaf
    int t;                           // Minimum degree
    int nodeId;                      // Unique node identifier

    BTreeNode(int _t, bool leaf, int id) : t(_t), isLeaf(leaf), nodeId(id) {
        keys.reserve(2 * t - 1);     // Reserve space for maximum keys
        children.reserve(2 * t);     // Reserve space for maximum children
    }
};

class Btree : public ColBasedTree {
private:
    BTreeNode* root;
    int t; // Minimum degree
    int nextNodeId; // To generate unique node IDs

    void saveNodeToFile(BTreeNode* node, const string& dir) {
        if (node == nullptr) return;

        // Ensure directory exists
        if (!std::filesystem::exists(dir)) {
            std::filesystem::create_directory(dir);
        }

        // Use node ID for consistent naming
        string fileName = dir + "/Node_" + to_string(node->nodeId) + ".txt";

        ofstream file(fileName);
        if (file.is_open()) {
            // Write the node's key(s)
            file << "Key: ";
            for (size_t i = 0; i < node->keys.size(); ++i) {
                file << node->keys[i];
                if (i < node->keys.size() - 1) file << ", ";
            }
            file << endl;

            // Write left children (if any)
            file << "Left: ";
            if (node->isLeaf || node->children.empty()) {
                file << "null";
            }
            else {
                BTreeNode* leftChild = node->children[0];
                for (const string& key : leftChild->keys) {
                    file << key << " ";
                }
            }
            file << endl;

            // Write right children (if any)
            file << "Right: ";
            if (node->isLeaf || node->children.size() < 2) {
                file << "null";
            }
            else {
                BTreeNode* rightChild = node->children.back();
                for (const string& key : rightChild->keys) {
                    file << key << " ";
                }
            }
            file << endl;

            // Additional node information
            file << "IsLeaf: " << (node->isLeaf ? "true" : "false") << endl;
            file << "NodeID: " << node->nodeId << endl;

            file.close();
            cout << "Node saved to file: " << fileName << endl;
        }
        else {
            cerr << "Error writing to file: " << fileName << endl;
        }
    }

    BTreeNode* insertNonFull(BTreeNode* node, const string& key, const string& dir) {
        int i = node->keys.size() - 1;

        if (node->isLeaf) {
            // Insert key into the leaf node
            node->keys.push_back("");
            while (i >= 0 && key < node->keys[i]) {
                node->keys[i + 1] = node->keys[i];
                i--;
            }
            node->keys[i + 1] = key;

            // Sort the keys in the leaf node
            sort(node->keys.begin(), node->keys.end());

            // Save the updated node
            saveNodeToFile(node, dir);
            return node;
        }

        // Find the child which is going to have the new key
        while (i >= 0 && key < node->keys[i]) {
            i--;
        }
        i++;

        // Check if the found child is full
        if (node->children[i]->keys.size() == (2 * t - 1)) {
            // Split the child
            BTreeNode* newNode = splitChild(node, i, dir);

            // Decide which of the two children is going to have the new key
            if (key > node->keys[i]) {
                i++;
            }
        }

        // Recursively insert into the appropriate child
        return insertNonFull(node->children[i], key, dir);
    }

    BTreeNode* splitChild(BTreeNode* parent, int i, const string& dir) {
        int t = this->t;
        BTreeNode* y = parent->children[i];
        BTreeNode* z = new BTreeNode(t, y->isLeaf, nextNodeId++);

        // Move the upper half of y to z
        z->keys.assign(y->keys.begin() + t, y->keys.end());
        y->keys.erase(y->keys.begin() + t, y->keys.end());

        // If not leaf, move the child pointers
        if (!y->isLeaf) {
            z->children.assign(y->children.begin() + t, y->children.end());
            y->children.erase(y->children.begin() + t, y->children.end());
        }

        // Add the middle key to the parent
        parent->keys.insert(parent->keys.begin() + i, y->keys[t - 1]);
        y->keys.erase(y->keys.begin() + t - 1);

        // Sort parent keys
        sort(parent->keys.begin(), parent->keys.end());

        // Add z as a child of parent
        parent->children.insert(parent->children.begin() + i + 1, z);

        // Save the modified nodes
        saveNodeToFile(y, dir);
        saveNodeToFile(z, dir);
        saveNodeToFile(parent, dir);

        return parent;
    }

    void printTreeRecursive(BTreeNode* node, int depth = 0) {
        if (node == nullptr) return;

        // Print indentation
        for (int i = 0; i < depth; i++) {
            cout << "  ";
        }

        // Print node keys
        cout << "Keys: ";
        for (const string& key : node->keys) {
            cout << key << " ";
        }
        cout << endl;

        // Recursively print children
        if (!node->isLeaf) {
            for (BTreeNode* child : node->children) {
                printTreeRecursive(child, depth + 1);
            }
        }
    }

    void clearTree(BTreeNode* node) {
        if (node == nullptr) return;

        // Recursively delete children
        if (!node->isLeaf) {
            for (BTreeNode* child : node->children) {
                clearTree(child);
            }
        }

        // Delete the node itself
        delete node;
    }

public:
    Btree(int _t) : root(nullptr), t(_t), nextNodeId(1) {}

    void insert(const string& key, const string& dir) override {
        if (root == nullptr) {
            root = new BTreeNode(t, true, nextNodeId++);
            root->keys.push_back(key);
            saveNodeToFile(root, dir);
        }
        else {
            // If root is full, grow the tree
            if (root->keys.size() == (2 * t - 1)) {
                BTreeNode* newRoot = new BTreeNode(t, false, nextNodeId++);
                newRoot->children.push_back(root);
                splitChild(newRoot, 0, dir);
                root = newRoot;
            }

            // Insert the new key
            insertNonFull(root, key, dir);
        }

        // Optional: save individual key file if needed
        string filePath = dir + "/" + key + "_key.txt";
        ofstream outFile(filePath);
        if (outFile.is_open()) {
            outFile << "Key: " << key << "\n";
            outFile.close();
        }
        else {
            cerr << "Error writing to key file: " << filePath << endl;
        }
    }

    void print() override {
        if (root != nullptr) {
            cout << "B-tree traversal:" << endl;
            // cout << "root:" << root->nodeId << endl;
            printTreeRecursive(root);
        }
        else {
            cout << "The tree is empty!" << endl;
        }
    }

    ~Btree() {
        // Recursively delete all nodes to prevent memory leaks
        clearTree(root);
    }
};