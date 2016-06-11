//
//  CommandLineParser.h
//  UNSPSC_Parser
//
//  Created by Nikos Detsikas on 10/4/15.
//  Copyright (c) 2015 Nikos Detsikas. All rights reserved.
//

#ifndef __UNSPSC_Parser__CommandLineParser__
#define __UNSPSC_Parser__CommandLineParser__

#include <stdio.h>
#include <string>

using namespace std;

typedef enum _Command
{
    LOAD_CATEGORY_TREE,
    CREATE_JSON,
    CREATE_CATEGORY_TREE,
    FIRST_LEVEL,
    TRAIN,
    TRAIN_LIST,
    CLASSIFY,
    CLASSIFY_LIST,
    INTERACTIVE_MODE,
    COMMAND_NONE
} Command;

class CommandLineParser
{
public:
    CommandLineParser();
    bool Parse(int argc, const char *argv[]);
private:
    bool usage(const char *argv0) const;

public:
    string UNSPSCFilename;
    string JSONFilename;
    string CategoryTreeFilename;
    string IDListFilename;
    string categoryTreePath;
    Command command;
    string item_id;
    
    
};

#endif /* defined(__UNSPSC_Parser__CommandLineParser__) */
