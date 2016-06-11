//
//  Controller.h
//  UNSPSC_Parser
//
//  Created by Nikos Detsikas on 7/6/15.
//  Copyright (c) 2015 Nikos Detsikas. All rights reserved.
//

#ifndef __UNSPSC_Parser__Controller__
#define __UNSPSC_Parser__Controller__

#include <stdio.h>
#include <string>

class Controller
{
public:
    static bool ignoreCRM114Classifiers;
    static std::string classifierOutputPath;
    static bool treeModified;
    static bool isLoaded;
    static bool forceSave;
    static bool unloadAfterSave;
};

#endif /* defined(__UNSPSC_Parser__Controller__) */
