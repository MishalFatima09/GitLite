#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
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

        if (command.find("init ") == 0) {
            string repoName, fileName, treeType;
            cout << "Enter repository name: ";
            cin >> repoName;
            cout << "Enter CSV file path: ";
            cin >> fileName;
            cout << "Enter tree type (AVL, BTree, RBTree): ";
            cin >> treeType;
            gitLite.initRepository(repoName, fileName, treeType);
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
        else if (command == "exit") {
            break;
        }
        else {
            cout << "Unknown command. Try init, list-repos, switch, delete, current-repo, print-tree, or exit." << endl;
        }
    }

    return 0;
}
