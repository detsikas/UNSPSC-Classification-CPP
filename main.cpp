//
//  main.cpp
//  UNSPSC_Parser
//
//  Created by Nikos Detsikas on 24/3/15.
//  Copyright (c) 2015 Nikos Detsikas. All rights reserved.
//

#include <iostream>
#include "UNSPSCFile.h"
#include "CategoriesTree.h"
#include <vector>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <streambuf>
#include "CommandLineParser.h"
#include "StopSetClass.h"
#include "libstemmer.h"
#include "Words.h"
#include "Tools.h"
#include "MyTypes.h"
#include "Controller.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/document.h"

using namespace std;
using namespace boost;

typedef enum _MenuOption{
    MENU_CREATE_CATEGORY_TREE=1,
    MENU_CREATE_STATISTICS,
    MENU_LOAD_CATEGORY_TREE,
    MENU_TRAIN_FILE,
    MENU_CLASSIFY_FILE,
    MENU_SAVE_UPDATE_CATEGORY_TREE,
    MENU_SAVE_CATEGORY_TREE,
    MENU_BROWSE_CATEGORY_TREE,
    MENU_CATEGORIZE_ITEM_FILE,
    MENU_EXIT
} MenuOption;

CategoryTree categoryTree;

static bool line2CategoryPieces(string line, CategoryPieces& categoryPieces)
{
    string separators("\t");
    vector<string> segments;
    boost::split(segments, line, boost::is_any_of(separators));
    vector<string>::const_iterator it = segments.begin();

    if (segments.size()<EXPECTED_CATEGORY_SIZE)
    {
        cerr<<"Bad category file format at line "<<line<<endl;
        return false;
    }
    
    unsigned int level;
    CategoryID numericalID = 0;
    string name;
    
    for (level=0; level<EXPECTED_CATEGORY_LEVELS; level++)
    {
        numericalID=numericalID*100+atoll(it->c_str());
        it++;
        name = Tools::stripDoubleQuotes(*it);
        *it++;
        categoryPieces.push_back(Category(name, numericalID));
    }
    
    return true;
}

static void openFile(UNSPSCFile& unspscFile, string const & UNSPSCfilename)
{
    if (!unspscFile.openFile(UNSPSCfilename))
    {
        cerr << "Unable to open file "<<UNSPSCfilename<<endl;
        exit(EXIT_FAILURE);
    }
}

static void parseFile(UNSPSCFile & unspscFile, CategoryTree& categoryTree, int numberOfLines = -1)
{
    string line;
    while (!(line=unspscFile.readLine()).empty() && (numberOfLines!=0))
    {
        CategoryPieces categoryPieces;
        if (!line2CategoryPieces(line, categoryPieces))
        {
            cerr<<"Error reading categories"<<endl;
            exit(EXIT_FAILURE);
        }
        categoryTree.AddCategory(categoryPieces);
        if (numberOfLines>0)
            numberOfLines--;
        string outputLine = "Nodes added: ";
        cout<<"                                                                                             \r";
        cout<<"Nodes added: "<<categoryTree.getNodeCount()<<" "<<"Leaves added: "<<categoryTree.getNumberOfLeaves()<<flush;
        
    }
    cout<<endl;
}

static bool serializeUNSPSC2JSON(CategoryTree const & categoryTree, string const & JSONFilename)
{
    FILE *fp;
    fp = fopen(JSONFilename.c_str(), "w");
    FileStream s(fp);
    PrettyWriter<FileStream> writer(s);     // Can also use Writer for condensed formatting
    
    categoryTree.SerializeJson(writer);
    
    fclose(fp);
    return true;
}

static void readCategoryTreeFile(string & categoryTreePath)
{
    Controller::classifierOutputPath = categoryTreePath;
    cout << "Reading category file and classifiers..."<<endl<<flush;
    string filename = categoryTreePath+"/categoryTree.data";
    ifstream ifs(filename.c_str());
    boost::archive::text_iarchive ia(ifs);
    ia>>categoryTree;
    cout <<endl<< "Done"<<endl<<flush;
}

static string createItemListString(item_id_list_t & item_id_list)
{
    item_id_list_t::const_iterator it;
    string result = "(";
    it = item_id_list.begin();
    while(it!=item_id_list.end())
    {
        result+=Tools::my_itoa(*it);
        it++;
        if (it!=item_id_list.end())
            result+=",";
    }
    result+=")";
    return result;
}

//The numbers must be separated by white space
static void readItemIdListFile(item_id_list_t & item_id_list, string & filename)
{
    ifstream ifs(filename.c_str());
    
    if (ifs.is_open())
    {
        item_id_t item_id;
        while (!ifs.eof())
        {
            if(ifs>>item_id)
            {
                cout<<item_id<<endl;
                item_id_list.push_back(item_id);
            }
            else if (!ifs.eof())
            {
                cerr<<"Bad id list file format"<<endl;
                exit(EXIT_FAILURE);
            }
        }
    }
}

static void createCategoryTreeInternal(string & unspscFilename)
{
    UNSPSCFile unspscFile;
    cout<<"Reading unspsc file ..."<<flush;
    openFile(unspscFile, unspscFilename);
    cout<<"Done"<<endl<<flush;
    
    cout<<"Creating category tree ..."<<endl<<flush;
    parseFile(unspscFile, categoryTree);
    cout<<"Done"<<endl<<flush;
    
    cout<<"Adding classifiers ..."<<flush;
    categoryTree.AddClassifiers();
    cout<<"Done"<<endl<<flush;
}

static void menuCreateCategoryTree()
{
    string unspscFileName;
    cout<<"Enter unspsc file: ";
    getline(cin, unspscFileName);
    createCategoryTreeInternal(unspscFileName);
    Controller::isLoaded = true;
}

static void saveCategoryTreeInternal(string & categoryTreePath)
{
    cout<<"Saving category tree and CRM114 classifiers ... "<<endl<<flush;
    string fullFilename = categoryTreePath+"/categoryTree.data";
    
    if (!Tools::my_mkdir(categoryTreePath))
    {
        cerr<<"Cannot make directory "<<categoryTreePath<<endl<<flush;
        exit(EXIT_FAILURE);
    }
    
    ofstream ofs(fullFilename.c_str());
    boost::archive::text_oarchive oa(ofs);
    oa<<categoryTree;
    cout <<endl<<"Done"<<endl;
}

static void trainFileInternal(string & trainFilename, Words & words)
{
    char readBuffer[5242880]; //5M

    FILE *fp = fopen(trainFilename.c_str(), "r");
    FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    Document d;
    d.ParseStream(is);
    fclose(fp);
    if (d.HasMember("items"))
    {
        Value const & itemsList = d["items"];
        for (Value::ConstValueIterator it=itemsList.Begin(); it!=itemsList.End(); it++)
        {
            if (it->HasMember("id") && it->HasMember("name") && it->HasMember("description"))
            {
                item_id_t itemID = (*it)["id"].GetUint64();
                string itemName = (*it)["name"].GetString();
                string processedItemName = words.processPhrase(itemName);
//                string itemDescription = (*it)["description"].GetString();
                item_t item(itemID, processedItemName);
                categoryTree.Train(item);
            }
            else
            {
                cerr<<"Bad item file format"<<endl;
                exit(EXIT_FAILURE);
            }
        }
    }
    else
    {
        cerr<<"Bad item file format"<<endl;
        exit(EXIT_FAILURE);
    }
}

static void categorizeJsonFile(string & filename)
{
    char fileBuffer[5242880]; //5M
    bool stop = false;
    
    FILE *fpr = fopen(filename.c_str(), "r");
    FileReadStream is(fpr, fileBuffer, sizeof(fileBuffer));
    Document d;
    d.ParseStream(is);
    fclose(fpr);
    if (d.HasMember("items"))
    {
        Value & itemsList = d["items"];
        Value::ValueIterator it = itemsList.Begin();
        while (!stop && it!=itemsList.End())
        {
            if (it->HasMember("id") && it->HasMember("name") && it->HasMember("description"))
            {
                if (!it->HasMember("category"))
                {
                    item_id_t itemID = (*it)["id"].GetUint64();
                    string itemName = (*it)["name"].GetString();
                    string description = (*it)["description"].GetString();
                    cout<<itemID<<" - "<<itemName<<" - "<<description<<endl;
                    CategoryID categoryId = categoryTree.Browse();
                    if (categoryId!=numeric_limits<CategoryID>::max())
                        it->AddMember("category", categoryId, d.GetAllocator());
                    else stop = true;
                }
            }
            else
            {
                cerr<<"Bad item file format"<<endl;
                exit(EXIT_FAILURE);
            }
            it++;
        }
    }
    else
    {
        cerr<<"Bad item file format"<<endl;
        exit(EXIT_FAILURE);
    }
    
    FILE *fpw = fopen(filename.c_str(), "w");
    FileWriteStream os(fpw, fileBuffer, sizeof(fileBuffer));
    PrettyWriter<FileWriteStream> writer(os);
    d.Accept(writer);
    fclose(fpw);
}


static void browseCategoryTreeInternal()
{
    categoryTree.Browse();
}

static void categorizeItemFileInternal()
{
    string itemsFile;
    cout<<"Enter items file: ";
    getline(cin, itemsFile);
    categorizeJsonFile(itemsFile);
}

static void createStatisticsInternal()
{
    categoryTree.TraverseCategoryTree();
    Statistics::meanNodeWidth/=((float)(categoryTree.getNodeCount()-categoryTree.getNumberOfLeaves()));
}

static void menuSaveCategoryTree()
{
    if (Controller::treeModified || Controller::forceSave)
    {
        string categoryTreePath;
        cout<<"Enter output path: ";
        cin>>categoryTreePath;
        Controller::classifierOutputPath = categoryTreePath;
        saveCategoryTreeInternal(categoryTreePath);
        Statistics::resetWriteStatistics();
    }
    else
    {
        cout<<"Tree not changed"<<endl;
    }
}

static void menuLoadCategoryTree()
{
    string categoryTreePath;
    cout<<"Enter category tree path: ";
    getline(cin, categoryTreePath);
    readCategoryTreeFile(categoryTreePath);
    Controller::isLoaded = true;

    Statistics::resetReadStatistics();
}

static void menuTrainFile()
{
    string greekPath = "./";
    string englishPath = "./";
    Words words(greekPath, englishPath);
    if (!Controller::isLoaded)
    {
        menuLoadCategoryTree();
    }
    cout<<"Enter json filename: ";
    string trainFilename;
    cin>>trainFilename;
    trainFileInternal(trainFilename, words);
    Controller::treeModified = true;
}

static void menuCreateStatistics()
{
    if (!Controller::isLoaded)
    {
        menuLoadCategoryTree();
    }
    createStatisticsInternal();
    Statistics::dumpStatistics();
}

static void menuBrowseCategoryTree()
{
    if (!Controller::isLoaded)
    {
        menuLoadCategoryTree();
    }
    browseCategoryTreeInternal();
}

static void menuCategorizeItemFile()
{
    if (!Controller::isLoaded)
    {
        menuLoadCategoryTree();
    }
    categorizeItemFileInternal();
}

static void interactiveMode()
{
    string menuOption;
    unsigned int menuOptionNumerical;
    while(1)
    {
        cout<<MENU_CREATE_CATEGORY_TREE<<". Create category tree"<<endl;
        cout<<MENU_CREATE_STATISTICS<<". Create statistics"<<endl;
        cout<<MENU_LOAD_CATEGORY_TREE<<". Load category tree"<<endl;
        cout<<MENU_TRAIN_FILE<<". Train file"<<endl;
        cout<<MENU_CLASSIFY_FILE<<". Classify file"<<endl;
        cout<<MENU_SAVE_UPDATE_CATEGORY_TREE<<". Save-update categoryTree"<<endl;
        cout<<MENU_SAVE_CATEGORY_TREE<<". Force save category tree"<<endl;
        cout<<MENU_BROWSE_CATEGORY_TREE<<". Browse category tree"<<endl;
        cout<<MENU_CATEGORIZE_ITEM_FILE<<". Categorize item file"<<endl;
        cout<<MENU_EXIT<<". Exit"<<endl;
        cout<<endl;
        do
        {
            cout<<"Make a choice (1-"<<MENU_EXIT<<"): ";
            getline(cin, menuOption);
            menuOptionNumerical = atoi(menuOption.c_str());
        } while (menuOptionNumerical>MENU_EXIT || menuOptionNumerical==0);
        switch (menuOptionNumerical) {
            case MENU_CREATE_CATEGORY_TREE:
                menuCreateCategoryTree();
                break;
            case MENU_CREATE_STATISTICS:
                menuCreateStatistics();
                break;
            case MENU_LOAD_CATEGORY_TREE:
                menuLoadCategoryTree();
                break;
            case MENU_TRAIN_FILE:
                menuTrainFile();
                break;
            case MENU_CLASSIFY_FILE:
                break;
            case MENU_SAVE_UPDATE_CATEGORY_TREE:
                menuSaveCategoryTree();
                break;
            case MENU_SAVE_CATEGORY_TREE:
                Controller::forceSave = true;
                menuSaveCategoryTree();
                break;
            case MENU_BROWSE_CATEGORY_TREE:
                menuBrowseCategoryTree();
                break;
            case MENU_CATEGORIZE_ITEM_FILE:
                menuCategorizeItemFile();
                break;
            case MENU_EXIT:
                exit(0);
                break;
            default:
                break;
        }
    }
}

int main(int argc, const char * argv[])
{
    CommandLineParser commandLineParser;
    
    if (!commandLineParser.Parse(argc, argv))
        return 1;
    
    string UNSPSCFilename = commandLineParser.UNSPSCFilename;
    string JSONFilename = commandLineParser.JSONFilename;
    string IDListFilename = commandLineParser.IDListFilename;
    string CategoryTreeFilename = commandLineParser.CategoryTreeFilename;
    string item_id = commandLineParser.item_id;
    string categoryTreePath = commandLineParser.categoryTreePath;

    switch (commandLineParser.command)
    {
        case CREATE_JSON:
        {
            UNSPSCFile unspscFile;
            openFile(unspscFile, UNSPSCFilename);
            
            parseFile(unspscFile, categoryTree);

            if (!serializeUNSPSC2JSON(categoryTree, JSONFilename))
            {
                return 1;
            }
            break;
        }
        case CREATE_CATEGORY_TREE:
        {
            createCategoryTreeInternal(UNSPSCFilename);
            saveCategoryTreeInternal(categoryTreePath);
            break;
        }
        case FIRST_LEVEL:
        {
            readCategoryTreeFile(categoryTreePath);
            categoryTree.PrintFirstLevel();
            categoryTree.PrintFirstLevelMap();
            break;
        }
        case LOAD_CATEGORY_TREE:
        {
            readCategoryTreeFile(categoryTreePath);
            break;
        }
        case TRAIN:
        {
            string greekPath = "./";
            string englishPath = "./";
            Words words(greekPath, englishPath);
            readCategoryTreeFile(categoryTreePath);
            bool treeModified = false;
            uint64_t id = 0; //not needed yet, it can be anything
            string name = ""; //TODO: it used to be read from the DB, it must now be read from the command line
            string processedName = words.processPhrase(name);
            item_t item(id, processedName);
            categoryTree.Train(item);
            treeModified = true;
            break;
        }
        case TRAIN_LIST:
        {
            string greekPath = "./";
            string englishPath = "./";
            Words words(greekPath, englishPath);
            readCategoryTreeFile(categoryTreePath);
            bool treeModified = false;
            vector<item_t> items; //it used to be read from the DB, it must now be read from a file
            vector<item_t>::const_iterator it;
            for (it=items.begin(); it!=items.end(); it++)
            {
                uint64_t id = it->first;
                string name = it->second;
                string processedName = words.processPhrase(name);
                item_t item(id, processedName);
                categoryTree.Train(item);
            }
            treeModified = true;
            break;
        }
        case CLASSIFY:
        {
            readCategoryTreeFile(categoryTreePath);
            break;
        }
        case CLASSIFY_LIST:
        {
            item_id_list_t item_id_list;
            readItemIdListFile(item_id_list, IDListFilename);
            readCategoryTreeFile(categoryTreePath);
            break;
        }
        case INTERACTIVE_MODE:
        {
            interactiveMode();
            break;
        }
        default:
            break;
    }
    
    return 0;
}
