// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
//

#include "logging.h"

#include <string>
#include <iostream>
#include <ostream>
#include <cassert>

using namespace std;
using namespace sru_logging;

Logging::Logging(const char* fname, int line, bool cond) : cond(cond) {
  if(!cond) cout << "ERROR";
  cout << "[" << fname << ":" << line << "] ";
}

Logging::~Logging(){
  cout << endl;
  assert(cond);
}

ostream& Logging::ostream(){
  return std::cout;
}
