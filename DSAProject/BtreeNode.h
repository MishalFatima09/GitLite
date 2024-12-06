#pragma once
#pragma once
#include <iostream>
using namespace std;

class Node {
public:
    int* keys;          // Array of keys
    Node** child;       // Array of child pointers
    int t;              // Minimum degree
    int count;          // Current number of keys
    bool leaf;          // True if the node is a leaf, false otherwise

    // Constructor
    Node(int t, bool leaf) {
        this->t = t;
        this->leaf = leaf;
        keys = new int[2 * t - 1];      // Maximum keys a node can have is (2*t - 1)
        child = new Node * [2 * t];       // Maximum children a node can have is (2*t)
        count = 0;                      // Initially, no keys in the node
    }

    // Destructor
    ~Node() {
        delete[] keys;
        for (int i = 0; i < count + 1; ++i) {
            delete child[i];
        }
        delete[] child;
    }

    // Traverses the subtree rooted at this node
    void traverse() {
        int i;
        for (i = 0; i < count; i++) {
            // If the node is not a leaf, traverse the child before the key
            if (!leaf) {
                child[i]->traverse();
            }
            cout << keys[i] << " ";
        }

        // Traverse the last child
        if (!leaf) {
            child[i]->traverse();
        }
    }

    // Splits the child `childIdx` of this node
    void splitting(int childIdx, Node* fullChild) {
        // Create a new node to hold (t-1) keys of `fullChild`
        Node* newNode = new Node(fullChild->t, fullChild->leaf);
        newNode->count = t - 1;

        // Copy the last (t-1) keys from `fullChild` to `newNode`
        for (int j = 0; j < t - 1; j++) {
            newNode->keys[j] = fullChild->keys[j + t];
        }

        // Copy the last t children from `fullChild` to `newNode` (if not a leaf)
        if (!fullChild->leaf) {
            for (int j = 0; j < t; j++) {
                newNode->child[j] = fullChild->child[j + t];
            }
        }

        // Reduce the number of keys in `fullChild`
        fullChild->count = t - 1;

        // Shift children of this node to make room for the new child
        for (int j = count; j >= childIdx + 1; j--) {
            child[j + 1] = child[j];
        }

        // Link the new child to this node
        child[childIdx + 1] = newNode;

        // Shift keys of this node to make room for the middle key of `fullChild`
        for (int j = count - 1; j >= childIdx; j--) {
            keys[j + 1] = keys[j];
        }

        // Copy the middle key of `fullChild` to this node
        keys[childIdx] = fullChild->keys[t - 1];

        // Increment count of keys in this node
        count++;
    }

    // Inserts a new key into the subtree rooted at this node
    void insertNonFull(int key) {
        int i = count - 1;

        // If this node is a leaf
        if (leaf) {
            // Find the location where the new key should be inserted
            while (i >= 0 && keys[i] > key) {
                keys[i + 1] = keys[i];
                i--;
            }

            // Insert the new key at the found location
            keys[i + 1] = key;
            count++;
        }
        else {
            // Find the child that will have the new key
            while (i >= 0 && keys[i] > key) {
                i--;
            }

            // Check if the found child is full
            if (child[i + 1]->count == 2 * t - 1) {
                // Split the child
                splitting(i + 1, child[i + 1]);

                // After splitting, the middle key of child[i+1] is moved to this node.
                // Decide which of the two children is going to have the new key
                if (keys[i + 1] < key) {
                    i++;
                }
            }
            child[i + 1]->insertNonFull(key);
        }
    }
};
