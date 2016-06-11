//
//  CommandLineParser.cpp
//  UNSPSC_Parser
//
//  Created by Nikos Detsikas on 10/4/15.
//  Copyright (c) 2015 Nikos Detsikas. All rights reserved.
//

#include "CommandLineParser.h"
#include <iostream>

CommandLineParser::CommandLineParser()
{
    command = COMMAND_NONE;
}

bool CommandLineParser::Parse(int argc, const char *argv[])
{
    if (argc<2)
        return usage(argv[0]);
    
    string argv1 = argv[1];
    
    if (!argv1.compare("createJson"))
    {
        if (argc!=4)
            return usage(argv[0]);
        
        command = CREATE_JSON;
        UNSPSCFilename = string(argv[2]);
        JSONFilename = string(argv[3]);
    }
    else if (!argv1.compare("createCategoryTree"))
    {
        if (argc!=4)
            return usage(argv[0]);
        
        command = CREATE_CATEGORY_TREE;
        UNSPSCFilename = string(argv[2]);
        categoryTreePath = string(argv[3]);
    }
    else if (!argv1.compare("loadCategoryTree"))
    {
        if (argc!=3)
            return usage(argv[0]);
        
        command = LOAD_CATEGORY_TREE;
        CategoryTreeFilename = string(argv[2]);
    }
    else if (!argv1.compare("firstLevel"))
    {
        if (argc!=3)
            return usage(argv[0]);
        
        command = FIRST_LEVEL;
        CategoryTreeFilename = string(argv[2]);
    }
    else if (!argv1.compare("classify"))
    {
        if (argc!=4)
            return usage(argv[0]);
        
        command = CLASSIFY;
        CategoryTreeFilename = string(argv[2]);
    }
    else if (!argv1.compare("classifyList"))
    {
        if (argc!=4)
            return usage(argv[0]);
        
        command = CLASSIFY_LIST;
        CategoryTreeFilename = string(argv[2]);
        IDListFilename = string(argv[3]);
    }
    else if (!argv1.compare("train"))
    {
        if (argc!=5)
            return usage(argv[0]);
        
        command = TRAIN;
        CategoryTreeFilename = string(argv[2]);
        categoryTreePath = string(argv[3]);
        item_id = argv[4];
    }
    else if (!argv1.compare("trainList"))
    {
        if (argc!=4)
            return usage(argv[0]);
        
        command = TRAIN_LIST;
        CategoryTreeFilename = string(argv[2]);
        IDListFilename = string(argv[3]);
    }
    else if (!argv1.compare("interactiveMode"))
    {
        if (argc!=2)
            return usage(argv[0]);
        
        command = INTERACTIVE_MODE;
    }
    else
    {
        return usage(argv[0]);
    }
    
    return true;
}

bool CommandLineParser::usage(const char* argv0) const
{
    cerr << "Usage: "<<argv0<<" createJson <UNSPSC filename>  <JSON filename>"<<endl;
    cerr << "Usage: "<<argv0<<" createCategoryTree <UNSPSC filename> <output path>"<<endl;
    cerr << "Usage: "<<argv0<<" loadCategoryTree <Category Tree filename>"<<endl;
    cerr << "Usage: "<<argv0<<" firstLevel <Category Tree filename>"<<endl;
    cerr << "Usage: "<<argv0<<" classify <Category Tree filename> <db item id>"<<endl;
    cerr << "Usage: "<<argv0<<" classifyList <Category Tree filename> <db item id list file>"<<endl;
    cerr << "Usage: "<<argv0<<" train <Category Tree filename> <category Tree path> <db item id>"<<endl;
    cerr << "Usage: "<<argv0<<" trainList <Category Tree filename> <db item id list file>"<<endl;
    cerr << "Usage: "<<argv0<<" interactiveMode"<<endl;
    return false;
}