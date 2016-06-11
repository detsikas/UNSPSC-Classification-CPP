//
//  Tools.h
//  UNSPSC_Parser
//
//  Created by Nikos Detsikas on 1/5/15.
//  Copyright (c) 2015 Nikos Detsikas. All rights reserved.
//

#ifndef __UNSPSC_Parser__Tools__
#define __UNSPSC_Parser__Tools__

#include <stdio.h>
#include <string>
#include <sstream>

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;

class Tools
{
public:
    template<typename T>
    static string my_itoa(T v)
    {
        ostringstream convert;
        convert << v;
        string s = convert.str();
        return s;
    }
    
    static string stripDoubleQuotes(string s)
    {
        // Remove all double-quote characters
        s.erase(
                remove( s.begin(), s.end(), '\"' ),
                s.end()
        );
        return s;
    }
    
    static bool my_mkdir(string & s)
    {
        struct stat statbuf;
        
        if ((stat(s.c_str(), &statbuf) == -1) || !S_ISDIR(statbuf.st_mode))
        {
            int error = mkdir(s.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
            if (error!=0)
            {
                return false;
            }
        }
        return true;
    }
};

#endif /* defined(__UNSPSC_Parser__Tools__) */
