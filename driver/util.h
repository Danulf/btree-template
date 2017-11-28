//  Created by Dana Groff on 10/11/17.
//  Copyright © 2017 Dana Groff. All rights reserved.
//
// btree_util.hxx
#include <iostream>

using std::cout;
using std::endl;

typedef std::pair<int, float> testPair; 
std::ostream& operator<<(std::ostream &os, const testPair &p) 
{
    os << "<" << p.first << ", " << p.second << ">"; 
    return os; 
}
