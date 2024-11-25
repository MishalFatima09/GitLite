//#include<iostream>
//using namespace std;
//#include"RBtree.h"
//
//int main()
//{
//    RBTree rbt;
//
//    // Inserting elements
//    rbt.insertValue(10);
//    rbt.insertValue(20);
//    rbt.insertValue(30);
//    rbt.insertValue(15);
//
//    // Inorder traversal
//    cout << "Inorder traversal:" << endl;
//    rbt.inorder(); // Output: 10 15 20 30
//
//    // Search for a node
//    cout << "\nSearch for 15: "
//        << (rbt.searchValue(15) != rbt.searchValue(0))
//        << endl; // Output: 1 (true)
//    cout << "Search for 25: "
//        << (rbt.searchValue(25) != rbt.searchValue(0))
//        << endl; // Output: 0 (false)
//    rbt.remove(20);
//
//    cout << "Inorder traversal:" << endl;
//    // Output: 10 15 20 30
//    rbt.inorder();
//
//    return 0;
//}