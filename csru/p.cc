// Programing Language SRU
// Copyright(C) 2005-2010 IKeJI
//

#include "p.h"

#include <iostream>
#include <cstdlib>
#include "native_proc.h"
#include "basic_object.h"
#include "logging.h"
#include "constants.h"
#include "sru_string.h"
#include "library.h"
#include "numeric.h"

using namespace sru;
using namespace std;

DEFINE_SRU_PROC(p) {
  ARGLEN(1);
  cout << args[0]->Inspect() << endl;
  return args[0];
}

DEFINE_SRU_PROC(puts) {
  ARGLEN(1);
  cout << SRUString::GetValue(args[0]).to_str() << endl;
  return args[0];
}

DEFINE_SRU_PROC(print) {
  ARGLEN(1);
  cout << SRUString::GetValue(args[0]).to_str();
  return args[0];
}

DEFINE_SRU_PROC(exit) {
  ARGLEN(1);
  int c = SRUNumeric::GetIntValue(args[0]);
  exit(c);
  return Library::Instance()->Nil();
}

namespace sru {

void SetupPrintFunction(const BasicObjectPtr& env){
  env->Set(sym::p(), CREATE_SRU_PROC(p));
  env->Set(sym::puts(), CREATE_SRU_PROC(puts));
  env->Set(sym::print(), CREATE_SRU_PROC(print));
  env->Set(sym::exit(), CREATE_SRU_PROC(exit));
}

} // namespace sru
