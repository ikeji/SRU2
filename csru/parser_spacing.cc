// Programing Language SRU
// Copyright(C) 2005-2010 IKeJI
// 

#include <string>
#include "native_proc.h"
#include "string.h"
#include "numeric.h"
#include "constants.h"
#include "library.h"
#include "parser_util.h"

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
  return CreateTrue(epos, Library::Instance()->Nil());
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
  return CreateTrue(epos, Library::Instance()->Nil());
}

DEFINE_SRU_PROC(lf){
  assert(args.size() > 2);
  const string& str = SRUString::GetValue(args[1]).to_str();
  int pos = SRUNumeric::GetValue(args[2]);
  if(str[pos] == '\r' && str[pos+1] == '\n'){
    return CreateTrue(pos+2, Library::Instance()->Nil());
  }
  if(str[pos] == '\r' || str[pos] == '\n'){
    return CreateTrue(pos+1, Library::Instance()->Nil());
  }
  return CreateFalse(pos, "End of line not found.");
}

DEFINE_SRU_PROC(eos){
  assert(args.size() > 2);
  const string& str = SRUString::GetValue(args[1]).to_str();
  int pos = SRUNumeric::GetValue(args[2]);
  if(pos == (int)str.length()){
    return CreateTrue(pos, Library::Instance()->Nil());
  }
  return CreateFalse(pos, "End of src not found.");
}

} // namespace sru_parser
