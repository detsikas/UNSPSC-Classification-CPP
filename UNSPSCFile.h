//
//  UNSPSCFile.h
//  UNSPSC_Parser
//
//  Created by Nikos Detsikas on 25/3/15.
//  Copyright (c) 2015 Nikos Detsikas. All rights reserved.
//

#ifndef __UNSPSC_Parser__UNSPSCFile__
#define __UNSPSC_Parser__UNSPSCFile__

#include <stdio.h>
#include <fstream>
#include <iostream>

using namespace std;

#define EXPECTED_CATEGORY_LEVELS 5
#define FULL_CATEGORY_NUMBER 1
#define EXPECTED_CATEGORY_SIZE (EXPECTED_CATEGORY_LEVELS*2+FULL_CATEGORY_NUMBER)

class UNSPSCFile
{
public:
    UNSPSCFile();
    UNSPSCFile(string);
    ~UNSPSCFile();
    string readLine();
    void resetFile();
    bool openFile(string);
private:
    ifstream infile;
};

#endif /* defined(__UNSPSC_Parser__UNSPSCFile__) */
