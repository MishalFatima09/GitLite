#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include "AVLclass.h"
#include"GitLite.h"
#include"Hasher.h"
using namespace std;
    


int main() {
    GitLite gitLite;
    string fileName;

    cout << "Enter the name of the CSV file to initialize the repository: ";
    cin >> fileName;

    gitLite.initRepository(fileName);

    return 0;
}
