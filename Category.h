//
//  Category.h
//  UNSPSC_Parser
//
//  Created by Nikos Detsikas on 24/3/15.
//  Copyright (c) 2015 Nikos Detsikas. All rights reserved.
//

#ifndef __UNSPSC_Parser__Category__
#define __UNSPSC_Parser__Category__

#include <stdio.h>
#include <string>
#include "rapidjson/prettywriter.h" // for stringify JSON
#include "rapidjson/filestream.h"   // wrapper of C stream for prettywriter as output
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>

#include "MyTypes.h"

using namespace std;
using namespace rapidjson;

class Category
{
public:
    //Constructors - Destructor
    Category();
    Category(string, CategoryID);
    Category(Category const &);
    ~Category();
    
    //Accessors
    CategoryID getID() const;
    string getName() const;
    size_t getItemsCount() const;
    
    bool compare(Category const &) const;
    
    //Json serialization
    template <typename Writer>
    void SerializeJson(Writer& writer) const
    {
        writer.String("id");
        writer.Uint64(categoryID);
        writer.String("name");
        writer.String(name.c_str(), (SizeType)name.length());
    }
    
    void Print() const;
    
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & name;
        ar & categoryID;
        ar & itemsCount;
    }
    string name;
    CategoryID categoryID;
    size_t itemsCount;
};

#endif /* defined(__UNSPSC_Parser__Category__) */
