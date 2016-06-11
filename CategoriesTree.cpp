//
//  CategoriesTree.cpp
//  UNSPSC_Parser
//
//  Created by Nikos Detsikas on 24/3/15.
//  Copyright (c) 2015 Nikos Detsikas. All rights reserved.
//


#include "CategoriesTree.h"

CategoryTree::CategoryTree():rootNode("UNSPSC Categories", 0)
{
    nodesInSubtree = 1;
    leavesInSubtree = 0;
}

CategoryTree::~CategoryTree()
{}

void CategoryTree::AddCategory(CategoryPieces & categoryList)
{
    nodesInSubtree += rootNode.AddCategory(categoryList);
    leavesInSubtree++;
}

void CategoryTree::PrintFirstLevel() const
{
    rootNode.PrintChildren();
}

void CategoryTree::PrintFirstLevelMap() const
{
    rootNode.getClassifier()->debugPrintClasses();
}

void CategoryTree::AddClassifiers()
{
    rootNode.AddClassifiers();
}

void CategoryTree::TraverseCategoryTree()
{
    rootNode.TraverseNode();
}

CategoryID CategoryTree::Browse()
{
    CategoryID categoryId;
    rootNode.Browse(categoryId);
    return categoryId;
}

CategoryTreeNode const & CategoryTree::getRootNode() const
{
    return rootNode;
}

CategoryTreeNode & CategoryTree::processRootNode()
{
    return rootNode;
}

void CategoryTree::Train(item_t & item)
{
    rootNode.Train(item);
}

void CategoryTree::Train(item_t const & item, CategoriesList & categoriesList, ClassesVector_t & resultVector)
{
    rootNode.Train(item, categoriesList, resultVector);
}

void CategoryTree::classifyItem(const item_t & item, vector<CRM114_MATCHRESULT> & resultVectror)
{
    rootNode.classifyItem(item, resultVectror);
}

unsigned int CategoryTree::getNodeCount() const
{
    return nodesInSubtree;
}

unsigned int CategoryTree::getNumberOfLeaves() const
{
    return leavesInSubtree;
}

unsigned int CategoryTree::countNodes() const
{
    return rootNode.countNodes();
}

//delete allocated nodes
