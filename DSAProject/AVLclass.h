#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include"Hasher.h"
using namespace std;



//AVL CLASS with additional member: hash value per node
struct AVLNode
{
    string key;        // Key for this node
    int hashValue;     // Hash value for this node
    AVLNode* left;
    AVLNode* right;
    int height;

    // Constructor
    AVLNode(string k, int h) : key(k), hashValue(h), left(nullptr), right(nullptr), height(1) {}
};


class AVLTree
{
private:
    AVLNode* root; // Root of the tree
    InstructorHash hasher; // Hashing utility

    int height(AVLNode* node)
    {
        return node ? node->height : 0;
    }

    // Helper function to update the hash for a node
    void updateHash(AVLNode* node)
    {
        if (!node) return;

        // Leaf node: hash is based on its key
        if (!node->left && !node->right)
        {
            node->hashValue = hasher.computeHash(node->key);
        }
        // Internal node: hash combines left and right child hashes
        else
        {
            string combined = (node->left ? to_string(node->left->hashValue) : "") +
                (node->right ? to_string(node->right->hashValue) : "");
            node->hashValue = hasher.computeHash(combined);
        }
    }

    int getBalance(AVLNode* node)
    {
        return node ? height(node->left) - height(node->right) : 0;
    }

    void updateHeight(AVLNode* node)
    {
        node->height = 1 + max(height(node->left), height(node->right));
    }

    // rotations; EVERYTIME U DO A ROTATION, YOU WILL ALSO UPDATE THE HASH. HENCE update Hash called for the new subtrees
    AVLNode* rightRotate(AVLNode* y)
    {
        AVLNode* x = y->left;
        AVLNode* T2 = x->right;

        // perform rotation
        x->right = y;
        y->left = T2;

        updateHeight(y);
        updateHeight(x);
        updateHash(y);
        updateHash(x);

        return x;
    }

    AVLNode* leftRotate(AVLNode* x) {
        AVLNode* y = x->right;
        AVLNode* T2 = y->left;

        // perform rotation
        y->left = x;
        x->right = T2;

        updateHeight(x);
        updateHeight(y);
        updateHash(x);
        updateHash(y);

        return y;
    }

    AVLNode* insertNode(AVLNode* node, const string& key)
    {
        if (node == nullptr) {
            int hash = hasher.computeHash(key); // Compute hash using Instructor Hash
            cout << "Creating node for key: " << key << ", Hash: " << hash << endl;
            return new AVLNode(key, hash);
        }

        if (key < node->key) {
            node->left = insertNode(node->left, key);
        }
        else if (key > node->key) {
            node->right = insertNode(node->right, key);
        }
        else {
            return node; // Duplicate keys are not allowed in the AVL tree
        }

        // Update height and hash of this ancestor node
        updateHeight(node);
        updateHash(node);

        // Get balance factor
        int balance = getBalance(node);

        // If the node becomes unbalanced, perform rotations

        // Left-Left Case
        if (balance > 1 && key < node->left->key) {
            return rightRotate(node);
        }

        // Right-Right Case
        if (balance < -1 && key > node->right->key) {
            return leftRotate(node);
        }

        // Left-Right Case
        if (balance > 1 && key > node->left->key) {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }

        // Right-Left Case
        if (balance < -1 && key < node->right->key) {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }

        return node;
    }

    // makes every node a txt file w key hash left right data in it
    void saveNodeToFile(AVLNode* node)
    {
        if (node == nullptr) return;

        string fileName = node->key + ".txt";
        ofstream file(fileName);
        if (file) {
            file << "Key: " << node->key << endl;
            file << "Hash: " << node->hashValue << endl;
            file << "Left: " << (node->left ? node->left->key : "NULL") << endl;
            file << "Right: " << (node->right ? node->right->key : "NULL") << endl;
            file.close();
            cout << "Node saved to file: " << fileName << endl;
        }

        //saving left n right subtrees
        saveNodeToFile(node->left);
        saveNodeToFile(node->right);
    }

public:
    AVLTree() : root(nullptr) {}

    // Insert a key into the AVL tree
    void insert(const string& key) {
        root = insertNode(root, key);
    }

    // Save the entire tree to .txt files
    void saveToFiles() {
        saveNodeToFile(root);
    }

    // Get the hash of the root node (Merkle Root Hash)
    int getRootHash() {
        return root ? root->hashValue : 0;
    }
};