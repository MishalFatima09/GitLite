#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
using namespace std;

// Instructor Hash Class
class InstructorHash {
public:
    // Compute Instructor Hash for an integer
    int computeHash(int number) {
        int hashValue = 1;
        while (number > 0) {
            int digit = number % 10; // Extract the last digit
            hashValue = (hashValue * digit) % 29; // Multiply and take modulo 29
            number /= 10; // Remove the last digit
        }
        return hashValue;
    }

    // Compute Instructor Hash for a string
    int computeHash(const string& str)
    {

        //int hashValue = 1;
        /*
        for (char c : str) {
            hashValue = (hashValue * static_cast<int>(c)) % 29; // Multiply ASCII value and take modulo 29
        }
        */
        int hashValue = 1; // Start with an initial hash value of 1
        for (size_t i = 0; i < str.length(); i++) {
            int asciiValue = static_cast<int>(str[i]); // Get the ASCII value of the character at index i
            hashValue = (hashValue * asciiValue) % 29; // Multiply and take modulo 29
        }
        return hashValue;
    }
};

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

// GitLite Class
class GitLite {
private:
    string fileName;
    string treeType;
    string hashMethod;
    int bTreeOrder = 0;
    vector<string> columnNames;

    // Helper function to split a line by commas
    vector<string> splitLine(const string& line) {
        vector<string> result;
        size_t start = 0, end = 0;

        while ((end = line.find(',', start)) != string::npos) {
            result.push_back(line.substr(start, end - start));
            start = end + 1;
        }
        result.push_back(line.substr(start)); // Add the last token
        return result;
    }

    void readCSVColumns()
    {
        ifstream file(fileName);
        if (!file) {
            cerr << "Error: Unable to open file " << fileName << endl;
            return;
        }

        // Read the first line to extract column names
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

        return choice - 1; // Convert to zero-based index
    }

    void initializeTreeStructure()
    {
        cout << "Choose tree type (AVL/B/Red-Black): ";
        cin >> treeType;

        if (treeType == "AVL" || treeType == "avl") {
            cout << "Choose hash method (1. Instructor Hash, 2. SHA-256): ";
            int hashChoice;
            do
            {
                cin >> hashChoice;
                if (hashChoice == 1)
                    hashMethod = "Instructor Hash";
                else
                    if (hashChoice == 2)
                        hashMethod = "SHA-256";
                    else
                        cout << "Invalid choice, try again: ";
            } while (hashChoice != 1 && hashChoice != 2);
        }
    }

public:
    void initRepository(const string& inputFileName)
    {
        fileName = inputFileName;

        cout << "Initializing repository with file: " << fileName << endl;

        // Step 1: Read column names from the CSV file
        readCSVColumns();
        if (columnNames.empty())
        {
            cerr << "Error: No columns found in the dataset." << endl;
            return;
        }

        // Step 2: Select the tree structure and hash method
        initializeTreeStructure();

        // Step 3: Allow the user to select a column for the tree
        int columnIndex = getColumnSelection();

        // Step 4: Create the Tree and insert keys 
        //AVL CASE:
        if (treeType == "AVL" || treeType == "avl") {
            AVLTree tree;
            ifstream file(fileName);
            if (!file) {
                cerr << "Error: Unable to open file " << fileName << endl;
                return;
            }

            // Skip header line
            string line;
            getline(file, line);

            // Insert keys from the selected column into the tree
            while (getline(file, line)) {
                vector<string> row = splitLine(line);
                if (columnIndex < static_cast<int>(row.size()))
                {
                    tree.insert(row[columnIndex]);
                }
            }

            //B TREE CASE:


            //RB TREE CASE:

            file.close();

            // Save tree to .txt files
            tree.saveToFiles();

            // Save root hash in metadata
            int rootHash = tree.getRootHash();
            ofstream repoFile("repository_meta.txt");
            repoFile << "File: " << fileName << endl;
            repoFile << "Tree Type: " << treeType << endl;
            repoFile << "Hash Method: " << hashMethod << endl;
            repoFile << "Selected Column: " << columnNames[columnIndex] << endl;
            repoFile << "Merkle Root Hash: " << rootHash << endl;
            repoFile.close();

            cout << "Repository initialized successfully with metadata saved." << endl;
            cout << "Merkle Root Hash: " << rootHash << endl;
        }
    }
};

int main() {
    GitLite gitLite;
    string fileName;

    // Event loop for command simulation
    cout << "Enter the name of the CSV file to initialize the repository: ";
    cin >> fileName;

    gitLite.initRepository(fileName);

    return 0;
}