#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <filesystem> // For directory operations

using namespace std;

class InstructorHash {
public:
    int computeHash(const string& data) {
        int hashValue = 1;
        for (char c : data) {
            hashValue = (hashValue * static_cast<int>(c)) % 29;
        }
        return hashValue;
    }

    int computeHash(int number) {
        int hashValue = 1;
        while (number > 0) {
            int digit = number % 10;
            hashValue = (hashValue * digit) % 29;
            number /= 10;
        }
        return hashValue;
    }
};

struct AVLNode {
    string key;        // Key for this node
    int hashValue;     // Hash value for this node
    AVLNode* left;     // Pointer to the left child
    AVLNode* right;    // Pointer to the right child
    int height;        // Height of the node

    AVLNode(string k, int h) : key(k), hashValue(h), left(nullptr), right(nullptr), height(1) {}
};

class MerkleAVLTree {
private:
    AVLNode* root;
    InstructorHash hasher;

    int height(AVLNode* node) {
        return node ? node->height : 0;
    }

    void updateHeight(AVLNode* node) {
        node->height = 1 + max(height(node->left), height(node->right));
    }

    void updateHash(AVLNode* node) {
        if (!node) return;

        if (!node->left && !node->right) {
            // Leaf node: hash is based on its key (the data itself)
            node->hashValue = hasher.computeHash(node->key);
        }
        else {
            // Internal node: combine hashes of left and right children
            string combinedHashes = (node->left ? to_string(node->left->hashValue) : "") +
                node->key + // This is where you might store a combined string (not the key itself)
                (node->right ? to_string(node->right->hashValue) : "");
            node->hashValue = hasher.computeHash(combinedHashes);
        }
    }

    AVLNode* rightRotate(AVLNode* y, const string& dir) {
        AVLNode* x = y->left;
        AVLNode* T2 = x->right;

        x->right = y;
        y->left = T2;

        updateHeight(y);
        updateHeight(x);
        updateHash(y);
        updateHash(x);

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
        updateHash(x);
        updateHash(y);

        saveNodeToFile(x, dir);
        saveNodeToFile(y, dir);

        return y;
    }

    AVLNode* insertNode(AVLNode* node, const string& key, const string& dir) {
        if (node == nullptr) {
            // For leaf nodes, set key to the data (e.g., "dwight")
            int hash = hasher.computeHash(key);
            AVLNode* newNode = new AVLNode(key, hash);
            saveNodeToFile(newNode, dir); // Save node immediately after creation
            return newNode;
        }

        if (key < node->key) {
            node->left = insertNode(node->left, key, dir);
        }
        else if (key > node->key) {
            node->right = insertNode(node->right, key, dir);
        }
        else {
            return node; // Duplicate keys are not allowed
        }

        updateHeight(node);
        updateHash(node); // Update the hash for the internal nodes

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

        saveNodeToFile(node, dir); // Save updated node
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
            file << "Hash: " << node->hashValue << endl;
            file << "Left: " << (node->left ? node->left->key : "NULL") << endl;
            file << "Right: " << (node->right ? node->right->key : "NULL") << endl;
            file.close();
            cout << "Node saved to file: " << fileName << endl;
        }
    }

    void printTree(AVLNode* node, string indent = "") {
        if (node == nullptr) return;

        // Print current node
        cout << indent << "Node Key: " << node->key << ", Hash: " << node->hashValue << endl;

        // Print left and right subtrees
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

    void insert(const string& key, const string& dir) {
        root = insertNode(root, key, dir);
    }

    int getRootHash() {
        return root ? root->hashValue : 0;
    }

    void print() {
        printTree(root, "");
    }
};

class GitLite {
private:
    string fileName;
    string treeType = "AVL";
    vector<string> columnNames;

    vector<string> splitLine(const string& line) {
        vector<string> result;
        size_t start = 0, end = 0;

        while ((end = line.find(',', start)) != string::npos) {
            result.push_back(line.substr(start, end - start));
            start = end + 1;
        }
        result.push_back(line.substr(start));
        return result;
    }

    void readCSVColumns() {
        ifstream file(fileName);
        if (!file) {
            cerr << "Error: Unable to open file " << fileName << endl;
            return;
        }

        string headerLine;
        if (getline(file, headerLine)) {
            columnNames = splitLine(headerLine);
        }
        file.close();
    }

    int getColumnSelection() {
        cout << "Available columns in the dataset:" << endl;
        for (size_t i = 0; i < columnNames.size(); ++i) {
            cout << i + 1 << ". " << columnNames[i] << endl;
        }

        int choice = 0;
        do {
            cout << "Select the column to construct the tree (1-" << columnNames.size() << "): ";
            cin >> choice;
        } while (choice < 1 || choice > static_cast<int>(columnNames.size()));

        return choice - 1;
    }

public:
    void initRepository(const string& inputFileName) {
        fileName = inputFileName;

        cout << "Initializing repository with file: " << fileName << endl;

        readCSVColumns();
        if (columnNames.empty()) {
            cerr << "Error: No columns found in the dataset." << endl;
            return;
        }

        int columnIndex = getColumnSelection();

        MerkleAVLTree tree;
        ifstream file(fileName);
        if (!file) {
            cerr << "Error: Unable to open file " << fileName << endl;
            return;
        }

        string line;
        getline(file, line);

        while (getline(file, line)) {
            vector<string> row = splitLine(line);
            if (columnIndex < static_cast<int>(row.size())) 
            {
                cout <<"insertion: " << row[columnIndex] << " ";
                tree.insert(row[columnIndex], "main-branch");
            }
        }

        int rootHash = tree.getRootHash();
        cout << "Merkle Root Hash: " << rootHash << endl;

        // Print the tree structure
        cout << "\nTree Structure: " << endl;
        tree.print();

        file.close();
    }
};

int main() {
    GitLite gitLite;
    string fileName;

    cout << "Enter the name of the CSV file to initialize the repository: ";
    cin >> fileName;

    gitLite.initRepository(fileName);

    return 0;
}
