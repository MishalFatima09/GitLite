#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include "AVLclass.h"

#include"Hasher.h"
using namespace std;


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
