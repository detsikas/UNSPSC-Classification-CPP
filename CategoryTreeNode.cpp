//
//  CategoryTreeNode.cpp
//  UNSPSC_Parser
//
//  Created by Nikos Detsikas on 24/3/15.
//  Copyright (c) 2015 Nikos Detsikas. All rights reserved.
//

#include "CategoryTreeNode.h"

CategoryTreeNode::CategoryTreeNode()
{
    classifier = NULL;
    isLeaf = false;
    leavesInSubtree = 0;
    nodesInSubtree = 1;
}

CategoryTreeNode::CategoryTreeNode(string n, CategoryID cid):category(n, cid)
{
    classifier = NULL;
    isLeaf = false;
    leavesInSubtree = 0;
    nodesInSubtree = 1;
}

void CategoryTreeNode::TraverseNode() const
{
    if (!isLeaf)
    {
        size_t childrenSize = children.size();
        if (childrenSize>Statistics::maxNodeWidth)
        {
            Statistics::maxNodeWidth = childrenSize;
            Statistics::nodeWithMaxWidth = category.getID();
        }
        Statistics::meanNodeWidth+=childrenSize;
        for (Children::const_iterator it=children.begin(); it!=children.end(); it++)
        {
            it->second->TraverseNode();
        }
        NodeWidthHistogram::iterator it = Statistics::nodeWidthHistogram.find(childrenSize);
        if (it!=Statistics::nodeWidthHistogram.end())
        {
            it->second++;
        }
        else
        {
            Statistics::nodeWidthHistogram[childrenSize] = 1;
        }
    }
}

bool CategoryTreeNode::Browse(CategoryID & categoryID)
{
    bool repeatList = true;
    if (!isLeaf)
    {
        string classOptionStr;
        int classOption = -2;
        int numberOfClasses = getClassifier()->getNumberOfClasses();
        while (repeatList)
        {
            while (classOption<-1 || classOption>numberOfClasses)
            {
                classifier->listClasses(true);
                getline(cin, classOptionStr);
                stringstream(classOptionStr)>>classOption;
            }
            if (classOption>0)
            {
                classOption--; //Convert to zero based
                CategoryID childClass = classifier->getCategoryIdOfClassAtIndex(classOption);
                repeatList = children[childClass]->Browse(categoryID);
                classOption = -1;
            }
            else if (classOption==0)
            {
                categoryID = 0;
                return true;
            }
            else //classoption is -1
            {
                categoryID= numeric_limits<CategoryID>::max();
                repeatList = false;
            }
        }
    }
    else
    {
        categoryID = category.getID();
    }
    
    return false;
}

CategoryTreeNode::~CategoryTreeNode()
{
    Children::iterator it;
    for (it=children.begin();it!=children.end();it++)
    {
        delete(it->second);
    }
}

unsigned int CategoryTreeNode::AddCategory(CategoryPieces &categoryList)
{
    unsigned int nodesAdded = 0;
    Category& category = categoryList.front();
    Children::iterator childIt = children.find(category.getID());
    CategoryTreeNode *childNode = NULL;
    if (childIt==children.end())
    {
        //child does not exist
        childNode = new CategoryTreeNode(category.getName(), category.getID());
        children.insert( ChildType(category.getID(), childNode) );
        nodesAdded++;
    }
    else
    {
        //child exists
        childNode = childIt->second;
    }
    
    categoryList.pop_front();
    
    if (!categoryList.empty())
    {
        nodesAdded += childNode->AddCategory(categoryList);
    }
    else
    {
        childNode->setIsLeaf();
    }
    
    //Every time we pass from a node, a leaf is added to its subtree
    leavesInSubtree++;
    nodesInSubtree+=nodesAdded;
    return nodesAdded;
}

Classifier const * CategoryTreeNode::getClassifier() const
{
    return (Classifier const *)classifier;
}

void CategoryTreeNode::unloadCRM114Classifier()
{
    classifier->unloadCRM114Classifier();
}

void CategoryTreeNode::AddClassifiers()
{
    if (children.size()!=0)
    {
        string name = Tools::my_itoa(category.getID());
        classifier = new Classifier(CLASSIFIER_FLAGS, name);
        classifier->setModified(true);
        classifier->SetNumberOfClasses((unsigned int)children.size());
    
        Children::const_iterator it;
        for (it=children.begin(); it!=children.end(); it++)
        {
            it->second->AddClassifiers();
            class_t _class(it->second->category.getID(), it->second->category.getName());
            classifier->AddClass(_class, 1);
        }
        classifier->SetDataBlocksSize();
        classifier->SetDataBlocks();
        classifier->crmClassifierBinarySize = classifier->p_cb->datablock_size;
    }
}

bool CategoryTreeNode::getIsLeaf() const
{
    return isLeaf;
}

void CategoryTreeNode::setIsLeaf()
{
    isLeaf = true;
}

Category const & CategoryTreeNode::getCategory() const
{
    return category;
}

Children const & CategoryTreeNode::getChildren() const
{
    return children;
}

Children & CategoryTreeNode::processChildren()
{
    return children;
}

void CategoryTreeNode::AddDefaultClassifier()
{
    classifier = new Classifier(CLASSIFIER_FLAGS);
    classifier->SetDataBlocksSize(8000000);
    classifier->SetDataBlocks();
}

void CategoryTreeNode::PrintChildren() const
{
    Children::const_iterator childIt;
    for (childIt = children.begin(); childIt!=children.end(); childIt++)
    {
        childIt->second->category.Print();
    }
}

void CategoryTreeNode::Train(item_t const & item)
{
    cout<<item.second<<endl;
    if (!isLeaf)
    {
        string classOptionStr;
        int classOption = -1;
        int numberOfClasses = getClassifier()->getNumberOfClasses();
        while (classOption<1 || classOption>numberOfClasses)
        {
            classifier->listClasses(false);
            getline(cin, classOptionStr);
            stringstream(classOptionStr)>>classOption;
        }
        classOption--; //Convert to zero based
        classifier->AddTrainingText(item, classOption);
        CategoryID childClass = classifier->getCategoryIdOfClassAtIndex(classOption);
        children[childClass]->Train(item);
    }
    else
    {
        itemList.push_back(item);
    }
}

void CategoryTreeNode::Train(item_t const & item, CategoriesList & categoriesList, ClassesVector_t & resultVector)
{
   if (!isLeaf)
   {
       unsigned int categoryIndex = categoriesList.front() - 1; //convert to zero based
       categoriesList.pop_front();
       int numberOfClasses = getClassifier()->getNumberOfClasses();
       if (categoryIndex>=numberOfClasses)
       {
           cerr<<"Wrong category index: "<<categoryIndex<<endl<<flush;
           exit(EXIT_FAILURE);
       }
       classifier->AddTrainingText(item, categoryIndex);
       CategoryID childClassID = classifier->getCategoryIdOfClassAtIndex(categoryIndex);
       string childClassName = classifier->getClassNameOfClassAtIndex(categoryIndex);
       resultVector.push_back(class_t(childClassID, childClassName));
       children[childClassID]->Train(item, categoriesList, resultVector);
   }
}

void CategoryTreeNode::classifyItem(const item_t & item, vector<CRM114_MATCHRESULT> & resultVector)
{
    if (!isLeaf)
    {
        int categoryIndex = classifier->classifyItem(item, resultVector);
        CategoryID childClass = classifier->getCategoryIdOfClassAtIndex(categoryIndex);
        children[childClass]->classifyItem(item, resultVector);
    }
    else
    {
        itemList.push_back(item);
    }
}

unsigned int CategoryTreeNode::countNodes() const
{
    unsigned int numberOfNodes = 1;
    Children::const_iterator childIt;
    for (childIt=children.begin(); childIt!=children.end(); childIt++)
    {
        numberOfNodes+= childIt->second->countNodes();
    }
    
    return numberOfNodes;
}

unsigned int CategoryTreeNode::getNodesInSubtree() const
{
    return nodesInSubtree;
}

unsigned int CategoryTreeNode::getLeavesInSubtree() const
{
    return leavesInSubtree;
}

item_list_t const & CategoryTreeNode::getItemList() const
{
    return itemList;
}