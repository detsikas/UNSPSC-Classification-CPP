//
//  Controller.cpp
//  UNSPSC_Parser
//
//  Created by Nikos Detsikas on 7/6/15.
//  Copyright (c) 2015 Nikos Detsikas. All rights reserved.
//

#include "Controller.h"


bool Controller::ignoreCRM114Classifiers = false;
std::string Controller::classifierOutputPath = "";
bool Controller::treeModified = false;
bool Controller::isLoaded = false;
bool Controller::forceSave = false;
bool Controller::unloadAfterSave = true;