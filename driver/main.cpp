//
//  main.cpp
//  test
//
//  Created by Dana Groff on 10/11/17.
//  Copyright © 2017 Dana Groff. All rights reserved.
//

#include <iostream>
#include <utility>
#include <time.h>
#include "util.h"
#include "../btree.hxx"


using std::cout;
using std::endl;
using namespace btree;


int main(int argc, const char * argv[]) {
    

    // this is a driver of the btree code 
    // it is NOT the best approach for testing
    // btrees; might be fine for stress testing
    // .TODO. write a good unit test
    //
    btree<int> tree;
    cout << tree << endl;
    
    if (!tree.remove(0))
        cout << "success, won't remove anything from a null tree" << endl;
    
    // fill the tree with randome int's
    srand ((int)time(NULL));
    
    // int tree
    for (int i = 0; i < 1000; ++i)
    {
        int val = rand()%100000;
        if (!tree.insert(val))
            cout << "int tree found value: " << val << endl;
    }
    cout << tree << endl << endl;
    
    // remove some random nodes
    for (int i = 0; i < 500; ++i)
    {
        int val = rand()%100000;
        if (tree.remove(val))
            cout << "int tree found value to remove: " << val << endl;
        else
            cout << "int tree NOT FOUND:" << val << endl;
    }
    cout << tree << endl << endl;
    
    // float tree
    btree<float> ftree;
    for (int i = 0; i < 1000; ++i)
    {
        float f = (float)(rand()%100000);
        if (f!=0)
            f = f/ (float)((rand()+1)%1000);
        if (!ftree.insert( f ))
            cout << "float tree found value: " << f << endl;
    }
    cout << ftree << endl << endl;
    
    // test a structure
    btree<testPair> pairTree;
    for (int i = 0; i < 1000; ++i)
        pairTree.insert( testPair((rand()%100000), (float)(rand()%100000) / (float)(rand()%1000)) );
    cout << pairTree << endl << endl;
    
    return 0;
}
