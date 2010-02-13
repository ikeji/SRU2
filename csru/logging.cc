// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
//

#include "logging.h"

#include <string>
#include <iostream>
#include <ostream>

using namespace std;
using namespace sru_logging;

Logging::Logging(const char* fname, int line){
  cout << "[" << fname << ":" << line << "] ";
}

Logging::~Logging(){
  cout << endl;
}

ostream& Logging::ostream(){
  return std::cout;
}
