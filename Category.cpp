//
//  Category.cpp
//  UNSPSC_Parser
//
//  Created by Nikos Detsikas on 24/3/15.
//  Copyright (c) 2015 Nikos Detsikas. All rights reserved.
//

#include "Category.h"


Category::Category(string n, CategoryID cid)
{
    name = n;
    categoryID = cid;
    itemsCount = 0;
}

Category::Category()
{
    categoryID = 0;
    itemsCount = 0;
}

Category::~Category()
{}

Category::Category(Category const &cat)
{
    name = cat.getName();
    categoryID = cat.getID();
    itemsCount = 0;
}

bool Category::compare(Category const & cat) const
{
    if (name!=cat.getName() || categoryID!=cat.getID() || itemsCount!=cat.getItemsCount())
        return false;
    return true;
}

size_t Category::getItemsCount() const
{
    return itemsCount;
}

CategoryID Category::getID() const
{
    return categoryID;
}

string Category::getName() const
{
    return name;
}

void Category::Print() const
{
    cout << name << "\t" << categoryID << endl;
}
