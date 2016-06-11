//
//  Classifier.cpp
//  UNSPSC_Parser
//
//  Created by Nikos Detsikas on 10/4/15.
//  Copyright (c) 2015 Nikos Detsikas. All rights reserved.
//

#include "Classifier.h"
#include <iostream>
#include <fstream>
#include <cmath>

#include "Tools.h"
#include "Controller.h"


Classifier::Classifier()
{
    modified = false;
    nextClassIndex = 0;
    p_cb = NULL;
    p_db = NULL;
    crmClassifierBinarySize = 0;
    err = CRM114_OK;
}

Classifier::Classifier(long long my_classifier_flags)
{
    nextClassIndex = 0;
    modified = false;
    crmClassifierBinarySize = 0;
    err = CRM114_OK;

    //Creating a CB (control block)
    if (((p_cb) = crm114_new_cb()) == NULL)
    {
        cerr << "Couldn't allocate!  Must exit!"<<endl;
        exit(0) ;
    };
    
    //Setting the classifier flags and style
    if ( crm114_cb_setflags (p_cb, my_classifier_flags ) != CRM114_OK)
    {
        cerr << "Couldn't set flags!  Must exit!"<<endl;
        exit(0);
    };
    
    //Setting the classifier defaults for this style classifier
    crm114_cb_setclassdefaults (p_cb);
    
    p_db = NULL;
}

Classifier::Classifier(long long my_classifier_flags, string & _name)
{
    nextClassIndex = 0;
    name = _name;
    modified = false;
    
    //Creating a CB (control block)
    if (((p_cb) = crm114_new_cb()) == NULL)
    {
        cerr << "Couldn't allocate!  Must exit!"<<endl;
        exit(0) ;
    };
    
    //Setting the classifier flags and style
    if ( crm114_cb_setflags (p_cb, my_classifier_flags ) != CRM114_OK)
    {
        cerr << "Couldn't set flags!  Must exit!"<<endl;
        exit(0);
    };
    
    //Setting the classifier defaults for this style classifier
    crm114_cb_setclassdefaults (p_cb);
}

ClassesVector_t const & Classifier::getClasses() const
{
    return (const ClassesVector_t &)classes;
}

void Classifier::AddClass(class_t & _class, unsigned int successFlag)
{
    strncpy(p_cb->_class[nextClassIndex].name, Tools::my_itoa(_class.first).c_str(), CLASSNAME_LENGTH);
    p_cb->_class[nextClassIndex].success = successFlag;
    classes.push_back(_class);
    nextClassIndex++;
}

void Classifier::AddClass(class_t & _class)
{
    strncpy(p_cb->_class[nextClassIndex].name, Tools::my_itoa(_class.first).c_str(), CLASSNAME_LENGTH);
    classes.push_back(_class);
    nextClassIndex++;
}

void Classifier::AddClass(string & name, unsigned int successFlag)
{
    strncpy(p_cb->_class[nextClassIndex].name, name.c_str(), CLASSNAME_LENGTH);
    p_cb->_class[nextClassIndex].success = successFlag;
    class_t _class(nextClassIndex, name);
    classes.push_back(_class);
    nextClassIndex++;
}

void Classifier::AddClass(string & name)
{
    strncpy(p_cb->_class[nextClassIndex].name, name.c_str(), CLASSNAME_LENGTH);
    class_t _class(nextClassIndex, name);
    classes.push_back(_class);
    nextClassIndex++;
}

void Classifier::SetNumberOfClasses(unsigned int noc)
{
    //Setting the number of classes
    p_cb->how_many_classes = (int)noc;
}

void Classifier::SetDataBlocksSize(size_t datablock_size)
{
    //Setting our desired starting space to a total of 8 megabytes
    //8000000 is a good value
    p_cb->datablock_size = datablock_size;
}

void Classifier::SetDataBlocksSize()
{
    p_cb->datablock_size = DEFAULT_DATABLOCK_SIZE;
}

void Classifier::SetDataBlocks()
{
    //Set up the CB internal state for this configuration
    crm114_cb_setblockdefaults(p_cb);
    
    //Use the CB to create a DB (data block)
    if ((p_db = crm114_new_db (p_cb)) == NULL)
    {
        cerr << "Couldn't create the datablock!  Must exit!" <<endl;
        exit(0);
    };
}

void Classifier::AddTrainingText(string const & text, unsigned int _class)
{
    //Training the classifier with text
    err = crm114_learn_text(&p_db, _class,
                            text.c_str(),
                            text.length() );
    if (!err)
        modified = true;
}

void Classifier::AddTrainingText(item_t const & item, unsigned int _class)
{
    //Training the classifier with text
    err = crm114_learn_text(&p_db, _class,
                            item.second.c_str(),
                            item.second.length());
    if (!err)
        modified = true;
}

void Classifier::ClassifyText(string text)
{
    CRM114_MATCHRESULT result;
    //Classifying text
    if ((err = crm114_classify_text(p_db, text.c_str(), text.length(), &result)) != CRM114_OK)
        exit (err);
}

void Classifier::ClassifyTextDebug(string text, string message)
{
    CRM114_MATCHRESULT result;
    //Classifying text
    if ((err = crm114_classify_text(p_db, text.c_str(), text.length(), &result)) == CRM114_OK)
        crm114_show_result(message.c_str(), &result);
    else
        exit (err);
}

void Classifier::ClassifyTextDebug(string text, string message, CRM114_MATCHRESULT* referenceResult)
{
    CRM114_MATCHRESULT result;
    //Classifying text
    if ((err = crm114_classify_text(p_db, text.c_str(), text.length(), &result)) != CRM114_OK)
        exit (err);
    else
    {
        if (!compareResult(&result, referenceResult))
        {
            cerr<<"Classification not matched";
            //exit(EXIT_FAILURE);
        }
    }
}

int Classifier::classifyItem(const item_t & item, vector<CRM114_MATCHRESULT> & resultVector)
{
    int categoryIndex = -1;
    CRM114_MATCHRESULT result;
    //Classifying text
    if ((err = crm114_classify_text(p_db, item.second.c_str(), item.second.length(), &result)) != CRM114_OK)
        exit (err);
    else
    {
        categoryIndex = result.bestmatch_index;
        resultVector.push_back(result);
    }
    
    return categoryIndex;
}

bool Classifier::compareResult(CRM114_MATCHRESULT *result, CRM114_MATCHRESULT *referenceResult)
{
    if (!myDecimalCompare(result->tsprob,referenceResult->tsprob) ||
        !myDecimalCompare(result->overall_pR,referenceResult->overall_pR) ||
        result->bestmatch_index!=referenceResult->bestmatch_index ||
        result->unk_features!=referenceResult->unk_features)
        return false;
    
    unsigned int i;
    for (i=0; i<result->how_many_classes; i++)
    {
        if (result->_class[i].documents!=referenceResult->_class[i].documents ||
        result->_class[i].features!=referenceResult->_class[i].features ||
        result->_class[i].hits!=referenceResult->_class[i].hits ||
        !myDecimalCompare(result->_class[i].prob,referenceResult->_class[i].prob) ||
        !myDecimalCompare(result->_class[i].pR,referenceResult->_class[i].pR))
        return false;
    }
    return true;
}

bool Classifier::myDecimalCompare(double a, double b)
{
    return ((abs(a-b)<0.001)?true:false);
}

void Classifier::Save(string & filename) const
{
    ofstream myFile(filename.c_str(), ios::out | ios::binary);
    myFile.write((char*)p_db, p_db->cb.datablock_size);
    myFile.close();
}

void Classifier::SaveText(string & filename) const
{
    int ret;
    remove(filename.c_str());
    ret = crm114_db_write_text (p_db, filename.c_str());
    if (ret!=CRM114_OK)
    {
        cerr<<"Could not write text classifier file "<<filename<<endl<<flush;
        exit(EXIT_FAILURE);
    }
}

void Classifier::RestoreText(string & filename)
{
    struct stat buffer;
    if  (stat(filename.c_str(), &buffer) == 0)
        p_db = crm114_db_read_text (filename.c_str());
    else
    {
        cerr << filename << " does not exist"<<endl;
        exit(EXIT_FAILURE);
    }
}

void Classifier::Restore(string & filename)
{
    ifstream myFile(filename.c_str(), ios::in | ios::binary);
    if (myFile)
    {
        p_db = (CRM114_DATABLOCK*)malloc(crmClassifierBinarySize);
        myFile.read((char*)p_db, crmClassifierBinarySize);
        myFile.close();
    }
}

void Classifier::debugPrintClasses() const
{
    ClassesVector_t::const_iterator it;
    int i = 0;
    cout<<"Format: map class id - map class name - actual class name"<<endl;
    for (it=classes.begin(); it!=classes.end(); it++)
    {
        cout<<it->first<<"-"<<it->second<<"-"<<p_db->cb._class[i++].name<<endl;
    }
}

void Classifier::listClasses(bool withBackTrackOption) const
{
    int numberOfClasses = p_db->cb.how_many_classes;
    if (numberOfClasses)
    {
        size_t i;
        for (i=0; i<numberOfClasses; i++)
        {
            cout<<(i+1)<<". "<<p_db->cb._class[i].name<<" - "<<getClassNameOfClassAtIndex((unsigned int)i)<<endl;
        }
        cout<<endl;
        cout<<"0. BackTrack (at level 0 means no categorization (category id = 0))"<<endl; //for example something with bad description
        cout<<"-1. Exit"<<endl;
    }
}

int Classifier::getNumberOfClasses() const
{
    return p_db->cb.how_many_classes;
}

void Classifier::setModified(bool m)
{
    modified = m;
}

bool Classifier::getModified() const
{
    return modified;
}

CategoryID Classifier::getCategoryIdOfClassAtIndex(unsigned int index) const
{
    return classes.at(index).first;
}

string Classifier::getClassNameOfClassAtIndex(unsigned int index) const
{
    return classes.at(index).second;
}

void Classifier::saveCRM114Classifier() const
{
    string pathName = Controller::classifierOutputPath+"/classifiers/";
    
    string classifierFileName = pathName+name;
    if (modified || Controller::forceSave)
    {
        if (!Tools::my_mkdir(pathName))
        {
            cerr<<"Cannot make directory "<<pathName<<endl<<flush;
            exit(EXIT_FAILURE);
        }
            
        SaveText(classifierFileName);
//        Save(classifierFileName);
    }
}

void Classifier::unloadCRM114Classifier()
{
    if (p_db)
    {
        free(p_db);
        //Do we have to free p_cb inside the  p_db? Does this cause memory leaks?
        p_db = NULL;
    }
    
    if (p_cb)
    {
        free(p_cb);
        p_cb = NULL;
    }
}

void Classifier::restoreCRM114Classifier()
{
    string pathName = Controller::classifierOutputPath+"/classifiers/";
    string classifierFileName = pathName+name;
//    Restore(classifierFileName);
    RestoreText(classifierFileName);
}

string Classifier::getName() const
{
    return name;
}

Classifier::ComparisonError Classifier::compare(Classifier const * cla) const
{
    if (name!=cla->name)
        return ERROR_NAME;
    else if (classes!=cla->classes)
        return ERROR_MEMBER_CLASSES;
    else if (crmClassifierBinarySize!=cla->crmClassifierBinarySize)
        return ERROR_BINARY_SIZE;
    else if (memcmp((char*)&(p_db->cb), (char*)&(cla->p_db->cb), sizeof(p_db->cb)))
        return ERROR_CB_CONTENTS;
    else if (memcmp((char*)p_db, (char*)cla->p_db, crmClassifierBinarySize))
        return ERROR_DB_CONTENTS;
    else
        return ERROR_NONE;
}

Classifier::~Classifier()
{
    if (p_db)
        free (p_db);

    if (p_cb)
        free (p_cb);
}