#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <filesystem> 
#include"AVLTree.h"
#include"RBtree.h"
using namespace std;


class GitLite {
private:
    struct Repository
    {
        string name;
        ColBasedTree* tree;
        string directory;
    };

    map<string, Repository> repositories; // Manage multiple repositories by name
    string currentRepository; // Name of the active repository

    //adding creation of tree

    ColBasedTree* createTree(const string& type) {
        if (type == "AVL") {
            return new MerkleAVLTree();
        }
        else if (type == "RBTree") {
            return new RBTree(); // Implement this class
        }
        else {
            cerr << "Unknown tree type. Defaulting to AVL." << endl;
            return new MerkleAVLTree();
        }
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
   /* void initRepository(const string& repoName, const string& inputFileName) {
        if (repositories.find(repoName) != repositories.end()) {
            cout << "Repository " << repoName << " already exists." << endl;
            return;
        }

        vector<string> columnNames;
        readCSVColumns(inputFileName, columnNames);
        if (columnNames.empty()) {
            cerr << "Error: No columns found in the dataset." << endl;
            return;
        }

        int columnIndex = getColumnSelection(columnNames);

        MerkleAVLTree tree;
        ifstream file(inputFileName);
        if (!file) {
            cerr << "Error: Unable to open file " << inputFileName << endl;
            return;
        }

        string line;
        getline(file, line); // Skip the header

        while (getline(file, line)) {
            vector<string> row = splitLine(line);
            if (columnIndex < static_cast<int>(row.size())) {
                tree.insert(row[columnIndex], repoName + "-branch"); // Use branch name as directory
            }
        }

        file.close();

        // Save repository details
        Repository newRepo = { repoName, tree, repoName + "-branch" };
        repositories[repoName] = newRepo;
        currentRepository = repoName;

        cout << "Initialized repository: " << repoName << endl;
        cout << "Merkle Root Hash: " << tree.getRootHash() << endl;
    }
    */

    void initRepository(const string& repoName, const string& inputFileName, const string& treeType) {
        if (repositories.find(repoName) != repositories.end()) {
            cout << "Repository " << repoName << " already exists." << endl;
            return;
        }

        ColBasedTree* tree = createTree(treeType);
        vector<string> columnNames;
        readCSVColumns(inputFileName, columnNames);
        if (columnNames.empty()) {
            cerr << "Error: No columns found in the dataset." << endl;
            delete tree; // Clean up memory
            return;
        }

        int columnIndex = getColumnSelection(columnNames);
        ifstream file(inputFileName);
        if (!file) {
            cerr << "Error: Unable to open file " << inputFileName << endl;
            delete tree; // Clean up memory
            return;
        }

        string line;
        getline(file, line); // Skip the header
        while (getline(file, line)) {
            vector<string> row = splitLine(line);
            if (columnIndex < static_cast<int>(row.size())) {
                tree->insert(row[columnIndex], repoName + "-branch");
            }
        }
        file.close();

        Repository newRepo = { repoName, tree, repoName + "-branch" };
        repositories[repoName] = newRepo;
        currentRepository = repoName;

        cout << "Initialized repository: " << repoName << endl;
        cout << "Merkle Root Hash: " << tree->getRootHash() << endl;
    }


    void listRepositories() {
        if (repositories.empty()) {
            cout << "No repositories initialized yet." << endl;
            return;
        }

        cout << "Repositories:" << endl;
        for (const auto& [name, repo] : repositories) {
            cout << "- " << name << endl;
        }
    }

    void switchRepository(const string& repoName) {
        if (repositories.find(repoName) == repositories.end()) {
            cout << "Repository " << repoName << " not found." << endl;
            return;
        }

        currentRepository = repoName;
        cout << "Switched to repository: " << repoName << endl;
    }

    void deleteRepository(const string& repoName) {
        auto it = repositories.find(repoName);
        if (it == repositories.end()) {
            cout << "Repository " << repoName << " not found." << endl;
            return;
        }

        // Remove directory from file system
        filesystem::remove_all(it->second.directory);

        repositories.erase(it);
        if (currentRepository == repoName) {
            currentRepository.clear();
        }
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

        // Check if the repository exists in the map
        if (repositories.find(currentRepository) == repositories.end()) {
            cout << "Repository not found." << endl;
            return;
        }

        // Polymorphic call to print the current repository's tree
        repositories[currentRepository].tree->print(); // Use pointer-to-base for flexibility
    }

};
