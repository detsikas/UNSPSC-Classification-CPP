//
//  Statistics.h
//  UNSPSC_Parser
//
//  Created by Nikos Detsikas on 3/6/15.
//  Copyright (c) 2015 Nikos Detsikas. All rights reserved.
//

#ifndef __UNSPSC_Parser__Statistics__
#define __UNSPSC_Parser__Statistics__

#include <stdio.h>
#include "MyTypes.h"
#include <map>

typedef map<size_t, size_t> NodeWidthHistogram;
class Statistics
{
public:
    static unsigned int totalNumberOfNodes;
    static unsigned int nodesStored;
    static unsigned int nodesRead;
    static size_t maxNodeWidth;
    static CategoryID nodeWithMaxWidth;
    static float meanNodeWidth;
    static NodeWidthHistogram nodeWidthHistogram;
    
    static void dumpStatistics();
    static void resetReadStatistics();
    static void resetWriteStatistics();
};

#endif /* defined(__UNSPSC_Parser__Statistics__) */
