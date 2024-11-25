#include <iostream>
using namespace std;

// Node structure for the Red-Black Tree
struct RBNode {
    int value;
    bool color; // Red = true, Black = false
    RBNode* leftChild;
    RBNode* rightChild;
    RBNode* parentNode;

    RBNode(int value)
        : value(value), color(true), leftChild(nullptr), rightChild(nullptr), parentNode(nullptr) {}
};

// Red-Black Tree class
class RBTree {
private:
    RBNode* root;
    RBNode* sentinel;

    // Rotate left function
    void rotateLeft(RBNode* node) {
        RBNode* temp = node->rightChild;
        node->rightChild = temp->leftChild;
        if (temp->leftChild != sentinel) {
            temp->leftChild->parentNode = node;
        }
        temp->parentNode = node->parentNode;
        if (node->parentNode == nullptr) {
            root = temp;
        }
        else if (node == node->parentNode->leftChild) {
            node->parentNode->leftChild = temp;
        }
        else {
            node->parentNode->rightChild = temp;
        }
        temp->leftChild = node;
        node->parentNode = temp;
    }

    // Rotate right function
    void rotateRight(RBNode* node) {
        RBNode* temp = node->leftChild;
        node->leftChild = temp->rightChild;
        if (temp->rightChild != sentinel) {
            temp->rightChild->parentNode = node;
        }
        temp->parentNode = node->parentNode;
        if (node->parentNode == nullptr) {
            root = temp;
        }
        else if (node == node->parentNode->rightChild) {
            node->parentNode->rightChild = temp;
        }
        else {
            node->parentNode->leftChild = temp;
        }
        temp->rightChild = node;
        node->parentNode = temp;
    }

    // Balance the Red-Black Tree after inserting a node
    void fixInsertion(RBNode* node) {
        while (node != root && node->parentNode->color == true) {
            if (node->parentNode == node->parentNode->parentNode->leftChild) {
                RBNode* uncle = node->parentNode->parentNode->rightChild;
                if (uncle->color == true) {
                    node->parentNode->color = false;
                    uncle->color = false;
                    node->parentNode->parentNode->color = true;
                    node = node->parentNode->parentNode;
                }
                else {
                    if (node == node->parentNode->rightChild) {
                        node = node->parentNode;
                        rotateLeft(node);
                    }
                    node->parentNode->color = false;
                    node->parentNode->parentNode->color = true;
                    rotateRight(node->parentNode->parentNode);
                }
            }
            else {
                RBNode* uncle = node->parentNode->parentNode->leftChild;
                if (uncle->color == true) {
                    node->parentNode->color = false;
                    uncle->color = false;
                    node->parentNode->parentNode->color = true;
                    node = node->parentNode->parentNode;
                }
                else {
                    if (node == node->parentNode->leftChild) {
                        node = node->parentNode;
                        rotateRight(node);
                    }
                    node->parentNode->color = false;
                    node->parentNode->parentNode->color = true;
                    rotateLeft(node->parentNode->parentNode);
                }
            }
        }
        root->color = false;
    }

    // Inorder traversal
    void inorderTraversal(RBNode* node) {
        if (node != sentinel) {
            inorderTraversal(node->leftChild);
            cout << node->value << " ";
            inorderTraversal(node->rightChild);
        }
    }

    // Search helper function
    RBNode* searchNode(RBNode* node, int value) {
        if (node == sentinel || node->value == value) {
            return node;
        }
        if (value < node->value) {
            return searchNode(node->leftChild, value);
        }
        return searchNode(node->rightChild, value);
    }
 

    // Delete function to remove a value from the Red-Black Tree
    void removeValue(int value) {
        RBNode* z = searchValue(value);  // Find the node to delete
        if (z == sentinel) {
            cout << "Value not found in the tree." << endl;
            return;
        }

        RBNode* y = z;
        RBNode* x;
        bool yOriginalColor = y->color;

        if (z->leftChild == sentinel) {
            x = z->rightChild;
            transplantNodes(z, z->rightChild);
        }
        else if (z->rightChild == sentinel) {
            x = z->leftChild;
            transplantNodes(z, z->leftChild);
        }
        else {
            y = treeMinimum(z->rightChild);
            yOriginalColor = y->color;
            x = y->rightChild;

            if (y->parentNode == z) {
                x->parentNode = y;
            }
            else {
                transplantNodes(y, y->rightChild);
                y->rightChild = z->rightChild;
                y->rightChild->parentNode = y;
            }

            transplantNodes(z, y);
            y->leftChild = z->leftChild;
            y->leftChild->parentNode = y;
            y->color = z->color;
        }

        // After deletion, we need to fix any violations of the Red-Black properties
        if (yOriginalColor == false) {
            fixDeletion(x);
        }

        delete z;
    }

    // Transplant helper function for node replacement
    void transplantNodes(RBNode* oldNode, RBNode* newNode) {
        if (oldNode->parentNode == nullptr) {
            root = newNode;
        }
        else if (oldNode == oldNode->parentNode->leftChild) {
            oldNode->parentNode->leftChild = newNode;
        }
        else {
            oldNode->parentNode->rightChild = newNode;
        }
        newNode->parentNode = oldNode->parentNode;
    }

    // Function to find the minimum node in a subtree (used during deletion)
    RBNode* treeMinimum(RBNode* node) {
        while (node->leftChild != sentinel) {
            node = node->leftChild;
        }
        return node;
    }

    // Fix the tree after deletion to restore Red-Black properties
    void fixDeletion(RBNode* x) {
        while (x != root && x->color == false) {
            if (x == x->parentNode->leftChild) {
                RBNode* sibling = x->parentNode->rightChild;
                if (sibling->color == true) {
                    sibling->color = false;
                    x->parentNode->color = true;
                    rotateLeft(x->parentNode);
                    sibling = x->parentNode->rightChild;
                }
                if (sibling->leftChild->color == false && sibling->rightChild->color == false) {
                    sibling->color = true;
                    x = x->parentNode;
                }
                else {
                    if (sibling->rightChild->color == false) {
                        sibling->leftChild->color = false;
                        sibling->color = true;
                        rotateRight(sibling);
                        sibling = x->parentNode->rightChild;
                    }
                    sibling->color = x->parentNode->color;
                    x->parentNode->color = false;
                    sibling->rightChild->color = false;
                    rotateLeft(x->parentNode);
                    x = root;
                }
            }
            else {
                RBNode* sibling = x->parentNode->leftChild;
                if (sibling->color == true) {
                    sibling->color = false;
                    x->parentNode->color = true;
                    rotateRight(x->parentNode);
                    sibling = x->parentNode->leftChild;
                }
                if (sibling->rightChild->color == false && sibling->leftChild->color == false) {
                    sibling->color = true;
                    x = x->parentNode;
                }
                else {
                    if (sibling->leftChild->color == false) {
                        sibling->rightChild->color = false;
                        sibling->color = true;
                        rotateLeft(sibling);
                        sibling = x->parentNode->leftChild;
                    }
                    sibling->color = x->parentNode->color;
                    x->parentNode->color = false;
                    sibling->leftChild->color = false;
                    rotateRight(x->parentNode);
                    x = root;
                }
            }
        }
        x->color = false;
    }


public:
    // Constructor
    RBTree() {
        sentinel = new RBNode(0);
        sentinel->color = false;
        sentinel->leftChild = sentinel->rightChild = sentinel;
        root = sentinel;
    }

    // Insert function
    void insertValue(int value) {
        RBNode* newNode = new RBNode(value);
        newNode->leftChild = sentinel;
        newNode->rightChild = sentinel;

        RBNode* parentNode = nullptr;
        RBNode* currentNode = root;

        // Standard BST insert
        while (currentNode != sentinel) {
            parentNode = currentNode;
            if (newNode->value < currentNode->value) {
                currentNode = currentNode->leftChild;
            }
            else {
                currentNode = currentNode->rightChild;
            }
        }

        newNode->parentNode = parentNode;

        if (parentNode == nullptr) {
            root = newNode;
        }
        else if (newNode->value < parentNode->value) {
            parentNode->leftChild = newNode;
        }
        else {
            parentNode->rightChild = newNode;
        }

        if (newNode->parentNode == nullptr) {
            newNode->color = false;
            return;
        }

        if (newNode->parentNode->parentNode == nullptr) {
            return;
        }

        fixInsertion(newNode);
    }

    // Inorder traversal
    void inorder() {
        inorderTraversal(root);
    }

    // Search function
    RBNode* searchValue(int value) {
        return searchNode(root, value);
    }

    void remove(int value)
    {
        removeValue(value);
    }
};