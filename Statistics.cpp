//
//  Statistics.cpp
//  UNSPSC_Parser
//
//  Created by Nikos Detsikas on 3/6/15.
//  Copyright (c) 2015 Nikos Detsikas. All rights reserved.
//

#include "Statistics.h"
#include <fstream>
#include <iostream>

using namespace std;

unsigned int Statistics::totalNumberOfNodes = 0;
unsigned int Statistics::nodesStored = 0;
unsigned int Statistics::nodesRead = 0;
size_t Statistics::maxNodeWidth = 0;
CategoryID Statistics::nodeWithMaxWidth = 0;
float Statistics::meanNodeWidth = 0;
NodeWidthHistogram Statistics::nodeWidthHistogram;

void Statistics::dumpStatistics()
{
    ofstream myfile ("statistics.txt");
    if (myfile.is_open())
    {
        myfile << "Max node width: "<<maxNodeWidth<<endl;
        myfile << "Node with max width: "<<nodeWithMaxWidth<<endl;
        myfile << "Mean node width: "<<meanNodeWidth<<endl;
        myfile << "Node width histogram: "<<endl;
        for (NodeWidthHistogram::iterator it= nodeWidthHistogram.begin(); it!=nodeWidthHistogram.end(); it++)
        {
            myfile <<"\t "<<it->first<<": "<<it->second<<endl;
        }
        myfile.close();
    }
    else cerr << "Unable to open file";
}

void Statistics::resetReadStatistics()
{
    nodesRead = 0;
}

void Statistics::resetWriteStatistics()
{
    nodesStored = 0;
}
