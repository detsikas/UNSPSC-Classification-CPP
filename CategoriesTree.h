//
//  CategoriesTree.h
//  UNSPSC_Parser
//
//  Created by Nikos Detsikas on 24/3/15.
//  Copyright (c) 2015 Nikos Detsikas. All rights reserved.
//

#ifndef __UNSPSC_Parser__CategoriesTree__
#define __UNSPSC_Parser__CategoriesTree__

#include <stdio.h>
#include "CategoryTreeNode.h"
#include "Statistics.h"

class CategoryTree
{
public:
    //Constructors - destructor
    CategoryTree();
    ~CategoryTree();
    
    //Insertion
    void AddCategory(CategoryPieces&);
    
    //Add classifiers
    void AddClassifiers();
    
    void TraverseCategoryTree();
    CategoryID Browse();
    
    CategoryTreeNode const & getRootNode() const;
    CategoryTreeNode & processRootNode();
    
    void classifyItem(item_t const &, vector<CRM114_MATCHRESULT> &);
    
    //Json serialization
    template <typename Writer>
    void SerializeJson(Writer& writer) const
    {
        rootNode.SerializeJson(writer);
    }
    
    void PrintFirstLevel() const;
    void PrintFirstLevelMap() const;
    
    void Train(item_t & item);
    void Train(item_t const & item, CategoriesList &, ClassesVector_t &);
    
    unsigned int getNodeCount() const;
    unsigned int getNumberOfLeaves() const;
    unsigned int countNodes() const;

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & nodesInSubtree;
        ar & leavesInSubtree;
        Statistics::totalNumberOfNodes = nodesInSubtree;
        ar & rootNode;
    }
    
    CategoryTreeNode rootNode;
    unsigned int nodesInSubtree;
    unsigned int leavesInSubtree;
};



#endif /* defined(__UNSPSC_Parser__CategoriesTree__) */
