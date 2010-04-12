// Programing Language SRU
// Copyright(C) 2005-2010 IKeJI
// 

#include <string>
#include "native_proc.h"
#include "string.h"
#include "numeric.h"
#include "constants.h"
#include "library.h"

using namespace sru;
using namespace std;

namespace sru_parser {

DEFINE_SRU_PROC(spc){
  assert(args.size() > 2);
  const string& str = SRUString::GetValue(args[1]).to_str();
  int pos = SRUNumeric::GetValue(args[2]);
  int epos = pos;
  LOG << "spc start: pos = " << epos;
  while(true){
    if(epos > (int)str.size()) break;
    if(str[epos] != ' ') break;
    epos++;
  }
  LOG << "spc result: epos = " << epos;
  BasicObjectPtr ret = BasicObject::New();
  ret->Set(sym::status(), Library::Instance()->True());
  ret->Set(sym::pos(), SRUNumeric::New(epos));
  return ret;
}

DEFINE_SRU_PROC(spc_or_lf){
  assert(args.size() > 2);
  const string& str = SRUString::GetValue(args[1]).to_str();
  int pos = SRUNumeric::GetValue(args[2]);
  int epos = pos;
  LOG << "spc start: pos = " << epos;
  while(true){
    if(epos > (int)str.size()) break;
    if(str[epos] != ' ' && str[epos] != '\r' && str[epos] != '\n') break;
    epos++;
  }
  LOG << "spc result: epos = " << epos;
  BasicObjectPtr ret = BasicObject::New();
  ret->Set(sym::status(), Library::Instance()->True());
  ret->Set(sym::pos(), SRUNumeric::New(epos));
  return ret;
}



} // namespace sru_parser