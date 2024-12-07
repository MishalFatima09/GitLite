#include "MyMap.h"  // Custom implementation of `MyMap`
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include "AVLTree.h"
#include"Btree.h"
#include "RBtree.h"

using namespace std;

class GitLite {
private:
    struct Repository {
        string name;
        ColBasedTree* tree;
        string directory;
    };

    //custom map implementation 

    MyMap<string, Repository> repositories; 

    //identifier of the CURRENT repository
    string currentRepository;              
    //file to store repository information, to allow loading
    const string metadataFile = "repositories_metadata.txt";

    // Create a tree instance
    ColBasedTree* createTree(const string& type, int t = 3) {
        if (type == "AVL") {
            return new MerkleAVLTree();
        }
        else if (type == "RBTree") {
            return new RBTree();
        }
        else if (type == "BTree") {
            return new Btree(t);
        }
        else {
            cerr << "Unknown tree type. Defaulting to AVL." << endl;
            return new MerkleAVLTree();
        }
    }

    // Save repository metadata to file
    void saveMetadata() {
        ofstream out(metadataFile);
        if (!out) {
            cerr << "Error: Unable to save metadata." << endl;
            return;
        }

        repositories.display([&out](const string& key, const Repository& repo) {
            out << key << "," << repo.directory << endl;
            });
        out.close();
    }






    // Load repository metadata from file
    void loadMetadata() {
        ifstream in(metadataFile);
        if (!in) {
            cerr << "No metadata file found. Starting fresh." << endl;
            return;
        }

        string line;
        while (getline(in, line)) {
            size_t pos = line.find(',');
            if (pos != string::npos) {
                string name = line.substr(0, pos);
                string directory = line.substr(pos + 1);
                repositories.insert(name, { name, nullptr, directory });
            }
        }
        in.close();
    }


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

    void readCSVColumns(const string& fileName, vector<string>& columnNames) {
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

    int getColumnSelection(const vector<string>& columnNames) {
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
    GitLite() {
        loadMetadata(); // Load repository metadata at startup
    }

    void initRepository(const string& repoName, const string& inputFileName, const string& treeType, int t) {
        Repository repo;
        if (repositories.get(repoName, repo)) {
            cout << "Repository " << repoName << " already exists." << endl;
            return;
        }

        ColBasedTree* tree = createTree(treeType);
        vector<string> columnNames;
        readCSVColumns(inputFileName, columnNames);
        if (columnNames.empty()) {
            cerr << "Error: No columns found in the dataset." << endl;
            delete tree;
            return;
        }

        int columnIndex = getColumnSelection(columnNames);
        ifstream file(inputFileName);
        if (!file) {
            cerr << "Error: Unable to open file " << inputFileName << endl;
            delete tree;
            return;
        }

        string line;
        getline(file, line); // Skip the header
        while (getline(file, line)) {
            vector<string> row = splitLine(line);
            if (columnIndex < static_cast<int>(row.size())) {
                tree->insert(row[columnIndex], repoName + "-repo");
            }
        }
        file.close();

        string directory = repoName + "-repo";
        if (!filesystem::exists(directory)) {
            filesystem::create_directory(directory);
        }

        Repository newRepo = { repoName, tree, directory };
        repositories.insert(repoName, newRepo);
        currentRepository = repoName;

        saveMetadata(); // Save metadata after initializing a repository
        cout << "Initialized repository: " << repoName << endl;
       
    }

    void listRepositories() {
        if (repositories.empty()) {
            cout << "No repositories initialized yet." << endl;
            return;
        }

        cout << "Repositories:" << endl;
        repositories.display([](const string& key, const Repository& repo) {
            cout << "- " << key << " (" << repo.directory << ")" << endl;
            });
    }


    void switchRepository(const string& repoName) {
        Repository repo;
        if (!repositories.get(repoName, repo)) {
            cout << "Repository " << repoName << " not found." << endl;
            return;
        }

        currentRepository = repoName;
        cout << "Switched to repository: " << repoName << endl;
    }

    void deleteRepository(const string& repoName) {
        Repository repo;
        if (!repositories.get(repoName, repo)) {
            cout << "Repository " << repoName << " not found." << endl;
            return;
        }

        filesystem::remove_all(repo.directory);
        repositories.remove(repoName);

        if (currentRepository == repoName) {
            currentRepository.clear();
        }

        saveMetadata();
        cout << "Deleted repository: " << repoName << endl;
    }

    void showCurrentRepository() {
        if (currentRepository.empty()) {
            cout << "No active repository." << endl;
        }
        else {
            cout << "Current repository: " << currentRepository << endl;
        }
    }

    void printCurrentTree() {
        if (currentRepository.empty()) {
            cout << "No active repository to display." << endl;
            return;
        }

        Repository repo;
        if (!repositories.get(currentRepository, repo)) {
            cout << "Repository not found." << endl;
            return;
        }

        repo.tree->print();
    }

    void createBranch(const string& branchName) {
        Repository repo;
        if (!repositories.get(currentRepository, repo)) { // Use MyMap's get method
            cout << "No active repository found. Initialize a repository first." << endl;
            return;
        }

        string repoDirectory = repo.directory;
        string branchPath = repoDirectory + "/" + branchName;

        if (filesystem::exists(branchPath)) {
            cout << "Branch '" << branchName << "' already exists." << endl;
            return;
        }

        filesystem::create_directory(branchPath);

        cout << "Branch '" << branchName << "' created successfully." << endl;
        saveMetadata(); // Save metadata after creating a branch
    }

    void checkoutBranch(const string& branchName) {
        Repository repo;
        if (!repositories.get(currentRepository, repo)) { // Use MyMap's get method
            cout << "No active repository found. Initialize a repository first." << endl;
            return;
        }

        string repoDirectory = repo.directory;
        string branchPath = repoDirectory + "/" + branchName;

        if (!filesystem::exists(branchPath)) {
            cout << "Branch '" << branchName << "' does not exist." << endl;
            return;
        }

        cout << "Switched to branch '" << branchName << "'." << endl;
    }


};
