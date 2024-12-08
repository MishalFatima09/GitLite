#include "MyMap.h"  // Custom implementation of `MyMap`
#include <iostream>
#include <fstream>
#include <string>
#include<ctime>
#include <filesystem>
#include "AVLTree.h"
#include "Btree.h"
#include "RBtree.h"

using namespace std;

class GitLite {
private:
    struct Repository {
        string name;
        ColBasedTree* tree;
        string directory;
        string currentBranch;
    };

    // Custom map implementation
    MyMap<string, Repository> repositories;

    // Identifier of the CURRENT repository
    string currentRepository;
    string currentPath;

    // File to store repository information, to allow loading
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

    string getBranchLogFilePath(const string& branchName) {
        // Construct the path for the log file based on the current branch
        return currentRepository + "-repo/" + branchName + "_log.txt";
    }

    void writeLog(const string& branchName, const string& logEntry) {
        string logFilePath = getBranchLogFilePath(branchName);
        ofstream logFile(logFilePath, ios::app); // Open file in append mode

        if (logFile.is_open()) {
            logFile << logEntry << endl;
            logFile.close();
            cout << "Log entry added to " << logFilePath << endl;
        }
        else {
            cerr << "Error: Unable to open log file " << logFilePath << endl;
        }
    }

    string getCurrentTime() {
        time_t now = time(0);
        struct tm timeInfo;

        // Use localtime_s instead of localtime to avoid the warning
        localtime_s(&timeInfo, &now); // Thread-safe version of localtime

        char buffer[80];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeInfo);  // Format the time

        return string(buffer);
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

        currentPath = inputFileName;

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

        vector<vector<string>> rows; // Store all rows for row count calculation
        string line;
        getline(file, line); // Skip the header
        while (getline(file, line)) {
            vector<string> row = splitLine(line);
            rows.push_back(row);
        }
        file.close();

        string directory = repoName + "-repo";
        if (!filesystem::exists(directory)) {
            filesystem::create_directory(directory);
        }

        // Insert rows into the tree
        int rowCount = 0;  // Initialize the row count
        for (const auto& row : rows) {
            if (columnIndex < static_cast<int>(row.size())) {
                // Insert into the tree with all four required parameters
                tree->insert(row[columnIndex], row, rowCount, directory);
                rowCount++;  // Increment row count after each insertion
            }
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

    void createBranch(const  string& branchName) {
        Repository repo;
        if (!repositories.get(currentRepository, repo)) {
             cout << "No active repository found. Initialize a repository first." <<  endl;
            return;
        }

         string repoDirectory = repo.directory;
         string branchPath = repoDirectory + "/" + branchName;

        if ( std::filesystem::exists(branchPath)) {
             cout << "Branch '" << branchName << "' already exists." <<  endl;
            return;
        }

        // Create the new branch directory
        std::filesystem::create_directory(branchPath);

        // Copy the repository's current contents into the branch folder
        try {
            for (const auto& file : std::filesystem::directory_iterator(repoDirectory)) {
                // Avoid copying the branch folder into itself
                if (file.path().filename() != branchName) {
                    string destination = branchPath + "/" + file.path().filename().string();

                    // Remove the destination file if it already exists
                    if (std::filesystem::exists(destination)) {
                        std::filesystem::remove(destination);
                         cout << "Removed existing file: " << destination <<  endl;
                    }

                    // Copy the file, overwriting if it already exists
                    std::filesystem::copy(file.path(), destination, std::filesystem::copy_options::overwrite_existing);
                    cout << "Copied: " << file.path() << " to " << destination <<  endl;
                }
            }

             cout << "Branch '" << branchName << "' created and files copied successfully." <<  endl;
        }
        catch (const  std::filesystem::filesystem_error& e) {
             cout << "Error copying files: " << e.what() <<  endl;
            return;
        }

        // Set the current branch to the newly created branch
        repo.currentBranch = branchName;
        saveMetadata();  // Save repository metadata after creating a branch
        cout << "Switched to branch '" << branchName << "'." <<  endl;
    }

    void checkoutBranch(const string& branchName)
    {
        Repository repo;
        if (!repositories.get(currentRepository, repo))
        {
            cout << "No active repository found. Initialize a repository first." << endl;
            return;
        }

        string repoDirectory = repo.directory;
        string branchPath = repoDirectory + "/" + branchName;

        if (!filesystem::exists(branchPath)) {
            cout << "Branch '" << branchName << "' does not exist." << endl;
            return;
        }

        // Switch to the selected branch (simply set the current branch name)
        repo.currentBranch = branchName;

        // Optionally, handle additional tasks such as setting up trees or reloading data if necessary

        cout << "Switched to branch '" << branchName << "'." << endl;
    }


    /////////////////    QUERY WORK    /////////////////

    ColBasedTree* getTree() {
        Repository repo;
        if (repositories.get(currentRepository, repo)) {
            return repo.tree;  // Return the tree of the current repository
        }
        return nullptr;  // If no current repository, return nullptr
    }

    // Query menu method
    void queryMenu() {
        int queryChoice;
        cout << "\n--- Queries Menu ---\n";
        cout << "1. SELECT\n";
        cout << "2. UPDATE\n";
        cout << "3. INSERT\n";
        cout << "4. DELETE\n";
        cout << "Enter your choice: ";
        cin >> queryChoice;

        // Get the tree from GitLite
        ColBasedTree* tree = getTree();
        if (tree == nullptr) {
            cout << "No repository is currently loaded.\n";
            return;
        }
        Repository repo;
        switch (queryChoice) {
        case 1:
            handleSelect();
            break;
        case 2:
            cout << "File" << currentPath << endl;
            handleUpdate(tree);
            break;
        case 3:
            handleInsert(tree);  // Pass the tree to handleInsert
            break;
        case 4:
            if (repositories.get(currentRepository, repo)) {
                handleDelete(repo.tree, repo.directory);
            }
            else {
                cout << "No active repository for deletion.\n";
            }
            break;
        default:
            cout << "Invalid query option." << endl;
        }
    }

    // Handle the SELECT query
    void handleSelect() {
        cout << "\n--- SELECT ---\n";
        // Logic for SELECT query (can be expanded based on actual query needs)
        cout << "Select records within a range or a single record.\n";
    }

    int getColumnIndex(const string& column, const string& fileName) {
        ifstream file(fileName);
        if (!file) {
            cerr << "Error: Unable to open file " << fileName << endl;
            return -1;
        }
        if (!file.is_open()) {
            cout << "filePath: " << fileName << endl;
            cerr << "Error: Unable to open CSV file " << fileName << endl;
            return -1;
        }

        string headerLine;
        if (getline(file, headerLine)) {
            vector<string> columnNames = splitLine(headerLine); // Split the header line
            auto it = find(columnNames.begin(), columnNames.end(), column);
            if (it != columnNames.end()) {
                return distance(columnNames.begin(), it);
            }
        }

        cerr << "Error: Column " << column << " not found in the CSV file.\n";
        return -1;
    }

    void handleUpdate(ColBasedTree* tree/*, const string& csvFilePath*/) {
        string setColumn, newValue, whereColumn, whereValue;

        cout << "\n--- UPDATE ---\n";

        // Input the column to update and its new value
        cout << "Enter column to update (e.g., age): ";
        cin >> setColumn;
        cout << "Enter new value: ";
        cin >> newValue;

        // Input the condition column and its value
        cout << "Enter condition column (e.g., name): ";
        cin >> whereColumn;
        cout << "Enter condition value: ";
        cin >> whereValue;

        // Get column indices using GitLite's method
        int columnIndex = getColumnIndex(setColumn, currentPath);
        int conditionIndex = getColumnIndex(whereColumn, currentPath);

        if (columnIndex == -1 || conditionIndex == -1)
        {
            cout << "Invalid column names provided.\n";
            return;
        }

        Repository repo;
        if (repositories.get(currentRepository, repo)) {
            tree->update(columnIndex, newValue, conditionIndex, whereValue, repo.directory);
            cout << "Updated " << setColumn << " to " << newValue
                << " for records where " << whereColumn << " = " << whereValue << endl;

            string logEntry = "UPDATE: " + whereColumn + " = " + whereValue + " updated " + setColumn + " to " + newValue + " at " + getCurrentTime();
            writeLog(currentRepository, logEntry);  // Log the update to the current branch's log file
        }
        else {
            cout << "Error: Current repository does not exist.\n";
        }
    }

    // Handle the INSERT query

    void handleInsert(ColBasedTree* tree) {
        string key;
        vector<string> row;
        int rowCount;

        cout << "\n--- INSERT ---\n";

        // Input the key
        cout << "Enter key for the new record: ";
        cin >> key;

        // Input the number of fields in the row
        cout << "Enter the number of fields in the row: ";
        cin >> rowCount;

        // Input the row data
        row.resize(rowCount);
        cout << "Enter the row data (space-separated): ";
        for (int i = 0; i < rowCount; ++i) {
            cin >> row[i];
        }

        Repository repo;
        if (repositories.get(currentRepository, repo)) {
            string repoDirectory = repo.directory;

            // Insert into the tree
            tree->insert(key, row, rowCount, repoDirectory);
            cout << "Inserted " << key << " into the tree with " << rowCount << " fields." << endl;

            string logEntry = "INSERT: " + key + " was added at " + getCurrentTime();
            writeLog(currentRepository, logEntry);  // Log the insert to the current branch's log file
        }
        else {
            cout << "Error: Current repository does not exist." << endl;
        }
    }



    // Handle the DELETE query
    void handleDelete(ColBasedTree* tree, const string& repoDirectory) {
        int choice;
        cout << "\n--- DELETE ---\n";
        cout << "1. Delete a single record\n";
        cout << "2. Delete records within a range\n";
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 1) {
            string key;
            cout << "Enter key of the record to delete: ";
            cin >> key;

            // Call the tree's remove function for a single key
            tree->remove(key, repoDirectory);
            cout << "Record with key \"" << key << "\" has been deleted.\n";

            string logEntry = "DELETE: Record with key " + key + " was deleted at " + getCurrentTime();
            writeLog(currentRepository, logEntry);  // Log the delete to the current branch's log file
        }
        else if (choice == 2) {
            string startKey, endKey;
            cout << "Enter the start key: ";
            cin >> startKey;
            cout << "Enter the end key: ";
            cin >> endKey;

            // Call the tree's range-based remove function
            tree->removeRange(startKey, endKey, repoDirectory);
            cout << "Records with keys between \"" << startKey << "\" and \"" << endKey << "\" have been deleted.\n";

            string logEntry = "DELETE RANGE: Records with keys between " + startKey + " and " + endKey + " were deleted at " + getCurrentTime();
            writeLog(currentRepository, logEntry);  // Log the range delete to the
        }
        else {
            cout << "Invalid choice. Aborting delete operation.\n";
        }
    }


};
