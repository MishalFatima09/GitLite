#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <filesystem> // Add this for file system operations
namespace fs = std::filesystem;
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


    //created a function to delete node 
    //JUST YOUR STANDARD NODE DELTION IN AVL ALONG WITH THE UPDATE HASH AND HEIGHT

    AVLNode* deleteNode(AVLNode* node, const string& key) {
        if (node == nullptr) {
            return node; // Key not found
        }

        // Step 1: Perform standard BST deletion
        if (key < node->key) {
            node->left = deleteNode(node->left, key);
        }
        else if (key > node->key) {
            node->right = deleteNode(node->right, key);
        }
        else {
            // Node found, handle deletion
            string fileName = node->key + ".txt";
            if (fs::exists(fileName)) {
                fs::remove(fileName); // Delete the corresponding file
                cout << "Deleted file: " << fileName << endl;
            }
            else {
                cout << "File not found: " << fileName << endl;
            }

            // Node with only one child or no child
            if ((node->left == nullptr) || (node->right == nullptr)) {
                AVLNode* temp = node->left ? node->left : node->right;

                // No child case
                if (temp == nullptr) {
                    temp = node;
                    node = nullptr;
                }
                else { // One child case
                    *node = *temp; // Copy the contents of the non-empty child
                }
                delete temp;
            }
            else {
                // Node with two children: Get the in-order successor
                AVLNode* temp = getMinValueNode(node->right);

                // Copy the in-order successor's data to this node
                node->key = temp->key;
                node->hashValue = temp->hashValue;

                // Delete the in-order successor
                node->right = deleteNode(node->right, temp->key);
            }
        }

        // If the tree had only one node then return
        if (node == nullptr) {
            return node;
        }

        // Step 2: Update height and hash of the current node
        updateHeight(node);
        updateHash(node);

        // Step 3: Get the balance factor
        int balance = getBalance(node);

        // Step 4: Rebalance the tree

        // Left-Left Case
        if (balance > 1 && getBalance(node->left) >= 0) {
            return rightRotate(node);
        }

        // Left-Right Case
        if (balance > 1 && getBalance(node->left) < 0) {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }

        // Right-Right Case
        if (balance < -1 && getBalance(node->right) <= 0) {
            return leftRotate(node);
        }

        // Right-Left Case
        if (balance < -1 && getBalance(node->right) > 0) {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }

        return node;
    }

    // Helper function to find the node with the smallest key in a subtree
    AVLNode* getMinValueNode(AVLNode* node) {
        AVLNode* current = node;

        // Loop down to find the leftmost leaf
        while (current->left != nullptr) {
            current = current->left;
        }
        return current;
    }


    // Update an existing node's key (and hash) in the AVL Tree
    //I dont exactly understand what they mean by updating but for now this function can be used to rename the txt file
    AVLNode* updateNode(AVLNode* node, const string& oldKey, const string& newKey) {
        if (node == nullptr) {
            return nullptr; // Node not found
        }

        // Locate the node to update
        if (oldKey < node->key) {
            node->left = updateNode(node->left, oldKey, newKey);
        }
        else if (oldKey > node->key) {
            node->right = updateNode(node->right, oldKey, newKey);
        }
        else {
            // Node found, update it
            cout << "Updating node: " << node->key << " to " << newKey << endl;

            // Rename the associated file
            string oldFileName = oldKey + ".txt";
            string newFileName = newKey + ".txt";

            if (fs::exists(oldFileName)) {
                fs::rename(oldFileName, newFileName); // Rename the file
                cout << "Renamed file: " << oldFileName << " to " << newFileName << endl;
            }
            else {
                cout << "File not found: " << oldFileName << endl;
            }

            // Update the key
            node->key = newKey;
            // Recompute the hash value for the updated key
            node->hashValue = hasher.computeHash(newKey);
            cout << "Updated hash: " << node->hashValue << endl;

            // Recalculate height and hash for all affected ancestors
            updateHeight(node);
            updateHash(node);
        }

        // Ensure the AVL tree remains balanced
        int balance = getBalance(node);

        // Left-Left Case
        if (balance > 1 && newKey < node->left->key) {
            return rightRotate(node);
        }

        // Right-Right Case
        if (balance < -1 && newKey > node->right->key) {
            return leftRotate(node);
        }

        // Left-Right Case
        if (balance > 1 && newKey > node->left->key) {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }

        // Right-Left Case
        if (balance < -1 && newKey < node->right->key) {
            node->right = rightRotate(node);
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


    void deletion(string& key)
    {

        root = deleteNode(root, key);
    }


    // Function to update starting from the root
    void update(const string& oldKey, const string& newKey) {
        root = updateNode(root, oldKey, newKey);
    }
};


//creating a tree as global
AVLTree tree;


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
            //AVLTree tree;
            ifstream file(fileName);
            if (!file) {
                cerr << "Error: Unable to open file " << fileName << endl;
                return;
            }

            // Skip header line
            string line;
            getline(file, line);
            system("cls");

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

    string deleteName;
    cout << "Enter the key of the file to delete: ";
    cin >> deleteName;
    //???
   // gitLite.tre
    tree.deletion(deleteName);

    string oldName, newName;
    cout << "Enter the key of the file to change: ";
    cin >> oldName;
    cout << "Enter the new name: ";
    cin >> newName;
    tree.update(oldName, newName);

    return 0;
}