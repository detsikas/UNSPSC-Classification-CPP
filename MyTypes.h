//
//  MyTypes.h
//  UNSPSC_Parser
//
//  Created by Nikos Detsikas on 1/5/15.
//  Copyright (c) 2015 Nikos Detsikas. All rights reserved.
//

#ifndef UNSPSC_Parser_MyTypes_h
#define UNSPSC_Parser_MyTypes_h

#include <string>
#include <vector>
#include <list>

using namespace std;

typedef unsigned long long item_id_t;
typedef unsigned long long CategoryID;
typedef CategoryID class_id_t;
typedef vector<item_id_t> item_id_list_t;
typedef pair<item_id_t,string> item_t;
typedef vector<item_t> item_list_t;
typedef pair<class_id_t, string> class_t;
typedef vector<class_t> ClassesVector_t;
typedef list<unsigned int> CategoriesList;

#endif
