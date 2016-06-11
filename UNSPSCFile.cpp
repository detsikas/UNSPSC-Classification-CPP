//
//  UNSPSCFile.cpp
//  UNSPSC_Parser
//
//  Created by Nikos Detsikas on 25/3/15.
//  Copyright (c) 2015 Nikos Detsikas. All rights reserved.
//

#include "UNSPSCFile.h"

UNSPSCFile::UNSPSCFile()
{}

UNSPSCFile::UNSPSCFile(string filename):infile(filename.c_str())
{
    if (!infile.is_open())
        cout << "Unable to open file "<<filename;
}

UNSPSCFile::~UNSPSCFile()
{
    if (infile.is_open())
        infile.close();
}

bool UNSPSCFile::openFile(string filename)
{
    infile.open(filename.c_str());
    return infile.is_open();
}

void UNSPSCFile::resetFile()
{
    infile.clear();
    infile.seekg( 0 );
}

string UNSPSCFile::readLine()
{
    string line;
    getline(infile, line, '\r');
    return line;
}