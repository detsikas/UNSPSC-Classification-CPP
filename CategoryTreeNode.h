//
//  CategoryTreeNode.h
//  UNSPSC_Parser
//
//  Created by Nikos Detsikas on 24/3/15.
//  Copyright (c) 2015 Nikos Detsikas. All rights reserved.
//

#ifndef __UNSPSC_Parser__CategoryTreeNode__
#define __UNSPSC_Parser__CategoryTreeNode__

#include <stdio.h>
#include <map>
#include <list>
#include "Category.h"
#include "Classifier.h"
#include <iomanip>
#include <locale>
#include <set>

#include "Tools.h"
#include "MyTypes.h"

#include <boost/serialization/list.hpp>
#include "Statistics.h"


class CategoryTreeNode;

typedef list<Category> CategoryPieces;
typedef pair<CategoryID, CategoryTreeNode*> ChildType;
typedef map<CategoryID, CategoryTreeNode*> Children;

class CategoryTreeNode
{
public:
    //Constructors - Destructor
    CategoryTreeNode();
    CategoryTreeNode(string, CategoryID);
    ~CategoryTreeNode();
    
    //Insertion
    unsigned int AddCategory(CategoryPieces&);
    
    //Add classifiers
    void AddClassifiers();
    void AddDefaultClassifier();
    void TraverseNode() const;
    bool Browse(CategoryID &);
    Classifier const * getClassifier() const;
    Category const & getCategory() const;
    Children const & getChildren() const;
    Children & processChildren();
    void unloadCRM114Classifier();
    
    //Json serialization
    template <typename Writer>
    void SerializeJson(Writer& writer) const
    {
        writer.StartObject();
        category.SerializeJson(writer);
        if (children.size())
        {
            writer.String("children");
            writer.StartArray();
            Children::const_iterator it;
            for (it=children.begin(); it!=children.end(); it++)
            {
                it->second->SerializeJson(writer);
            }
            writer.EndArray();
        }
        writer.EndObject();
    }
    
    void PrintChildren() const;
    unsigned int getLeavesInSubtree() const;
    unsigned int getNodesInSubtree() const;
    unsigned int countNodes() const;
    item_list_t const & getItemList() const;
    bool getIsLeaf() const;
    void setIsLeaf();
    
    void Train(item_t const &);
    void Train(item_t const &, CategoriesList &, ClassesVector_t &);
    
    void classifyItem(item_t const &, vector<CRM114_MATCHRESULT> &);
    
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        if (Archive::is_saving::value)
        {
            Statistics::nodesStored++;
            cout<<"                                                \r";
            cout<<(int)(((float)Statistics::nodesStored/(float)Statistics::totalNumberOfNodes)*100)<<"%"<<flush;
        }
        else
        {
            Statistics::nodesRead++;
            cout<<"                                                \r";
            cout<<(int)(((float)Statistics::nodesRead/(float)Statistics::totalNumberOfNodes)*100)<<"%"<<flush;
        }
        ar & category;
        ar & classifier;
        ar & isLeaf;
        ar & children;
        ar & itemList;
        ar & leavesInSubtree;
        ar & nodesInSubtree;
    }

    Category category;
    Children children;
    Classifier* classifier;
    item_list_t itemList;
    bool isLeaf;
    unsigned int leavesInSubtree;
    unsigned int nodesInSubtree;
};

#endif /* defined(__UNSPSC_Parser__CategoryTreeNode__) */
