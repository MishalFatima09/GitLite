#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem> // For directory operations
#include"AVLTree.h"
#include"Hasher.h"
#include"GitLite.h"

using namespace std;


int main() {
    GitLite gitLite;

    while (true) {
        string command;
        cout << "> ";
        getline(cin, command);
        string fileName;
        if (command.find("init ") == 0) {
            string repoName, treeType;
            int t = 3; // Default minimum degree for BTree

            // Collect repository information
            cout << "Enter repository name: ";
            cin >> repoName;
            cout << "Enter CSV file path: ";
            cin >> fileName;
            cout << "Enter tree type (AVL, BTree, RBTree): ";
            cin >> treeType;

            // Handle BTree-specific input
            if (treeType == "BTree") {
                cout << "Enter minimum degree for BTree (t): ";
                cin >> t;
                if (t < 2) {
                    cerr << "Minimum degree must be at least 2. Defaulting to 3." << endl;
                    t = 3;
                }
            }

            // Call the appropriate GitLite method
            gitLite.initRepository(repoName, fileName, treeType, t);
        }
        else if (command == "list-repos") {
            gitLite.listRepositories();
        }
        else if (command.find("switch ") == 0) {
            string repoName = command.substr(7);
            gitLite.switchRepository(repoName);
        }
        else if (command.find("delete ") == 0) {
            string repoName = command.substr(7);
            gitLite.deleteRepository(repoName);
        }
        else if (command == "current-repo") {
            gitLite.showCurrentRepository();
        }
        else if (command == "print-tree") {
            gitLite.printCurrentTree();
        }
        else if (command.find("branch ") == 0) {
            string branchName = command.substr(7);
            gitLite.createBranch(branchName);
        }
        else if (command.find("checkout ") == 0) {
            string branchName = command.substr(9);
            gitLite.checkoutBranch(branchName);
        }
        else if (command == "queries") {
            cout << fileName << endl;
            gitLite.queryMenu();  
        }
        else if (command == "exit") {
            break;
        }
        else {
            cout << "Unknown command. Try init, list-repos, switch, delete, current-repo, queries, print-tree, or exit." << endl;
        }
    }

    return 0;
}




