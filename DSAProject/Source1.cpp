#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <filesystem> // For directory operations


namespace fs = std::filesystem;
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
        updateHash(node); // Update the hash for the internal nodes

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
        if (balance > 1 && key < node->left->key) {
            return rightRotate(node, dir);
        }

        // Right-Right Case
        if (balance < -1 && key > node->right->key) {
            return leftRotate(node, dir);
        }

        // Left-Right Case
        if (balance > 1 && key > node->left->key) {
            node->left = leftRotate(node->left, dir);
            return rightRotate(node, dir);
        }

        // Right-Left Case
        if (balance < -1 && key < node->right->key) {
            node->right = rightRotate(node->right, dir);
            return leftRotate(node, dir);
        }

        saveNodeToFile(node, dir); // Save updated node
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
