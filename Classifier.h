//
//  Classifier.h
//  UNSPSC_Parser
//
//  Created by Nikos Detsikas on 10/4/15.
//  Copyright (c) 2015 Nikos Detsikas. All rights reserved.
//

#ifndef __UNSPSC_Parser__Classifier__
#define __UNSPSC_Parser__Classifier__

#include <stdio.h>
#include <string>
#include <map>
#include "MyTypes.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>

#include <fstream>

#include "Controller.h"

using namespace std;

extern "C" {
#include "crm114_sysincludes.h"
#include "crm114_config.h"
#include "crm114_structs.h"
#include "crm114_lib.h"
#include "crm114_internal.h"
}

//#define CLASSIFIER_FLAGS (CRM114_DEFAULT | CRM114_STRING)
#define CLASSIFIER_FLAGS (CRM114_OSB | CRM114_UNIQUE | CRM114_MICROGROOM)
//datablock size does not really matter as each classifier sets its own when set defaults is called
#define DEFAULT_DATABLOCK_SIZE 8000
class Classifier
{
public:
    
    typedef enum _ComparisonError{
        ERROR_NONE,
        ERROR_NAME,
        ERROR_MEMBER_CLASSES,
        ERROR_BINARY_SIZE,
        ERROR_CB_CONTENTS,
        ERROR_DB_CONTENTS
    }ComparisonError;
    
    Classifier();
    Classifier(long long);
    Classifier(long long, string &);
    ~Classifier();
    
    void SetNumberOfClasses(unsigned int);
    void SetDataBlocks();
    void SetDataBlocksSize(size_t);
    void SetDataBlocksSize();
    
    void AddClass(class_t&, unsigned int);
    void AddClass(class_t&);
    void AddClass(string &, unsigned int);
    void AddClass(string &);
    
    void AddTrainingText(string const &, unsigned int);
    void AddTrainingText(item_t const &, unsigned int);
    
    void ClassifyText(string);
    void ClassifyTextDebug(string, string);
    void ClassifyTextDebug(string text, string message, CRM114_MATCHRESULT* referenceResult);
    
    int classifyItem(item_t const &, vector<CRM114_MATCHRESULT> &);
    
    //Unsafe to use binary save and restore, because of floating point operations
    void Save(string&) const;
    void Restore(string&);

    void SaveText(string&) const;
    void RestoreText(string&);
    
    void listClasses(bool withBackTrackOption) const;
    void debugPrintClasses() const;
    int getNumberOfClasses() const;
    
    CategoryID getCategoryIdOfClassAtIndex(unsigned int) const;
    string getClassNameOfClassAtIndex(unsigned int) const;
    
    void saveCRM114Classifier(void) const;
    void restoreCRM114Classifier(void);
    
    void unloadCRM114Classifier(void);
    
    void setModified(bool);
    bool getModified() const;
    
    ComparisonError compare(Classifier const *) const;
    
    string getName() const;
    ClassesVector_t const & getClasses() const;
    
    CRM114_CONTROLBLOCK *p_cb;
    CRM114_DATABLOCK *p_db;
    size_t crmClassifierBinarySize;
    
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & classes;
        ar & name;
        
        if (Archive::is_saving::value)
        {
            crmClassifierBinarySize = p_db->cb.datablock_size;
            ar & crmClassifierBinarySize;
            if (!Controller::ignoreCRM114Classifiers && (modified || Controller::forceSave))

            {
                saveCRM114Classifier();
                //This is for memory consumption purposes. For some reason, if the classifier is not unloaded after being saved,
                //the memory used during the save is not freed
                if (Controller::unloadAfterSave)
                    unloadCRM114Classifier();
            }
        }
        else
        {
            ar & crmClassifierBinarySize;
            if (!Controller::ignoreCRM114Classifiers)
            {
                restoreCRM114Classifier();
            }
        }
    }
    
    bool compareResult(CRM114_MATCHRESULT *result, CRM114_MATCHRESULT* referenceResult);
    bool myDecimalCompare(double, double);
    
    ClassesVector_t classes;
    string name;

    CRM114_ERR err;
    unsigned int nextClassIndex;
    bool modified;
};

#endif /* defined(__UNSPSC_Parser__Classifier__) */
