// Programing Language SRU
// Copyright(C) 2005-2010 IKeJI
// 

#include <string>
#include "native_proc.h"
#include "string.h"
#include "numeric.h"
#include "library.h"
#include "logging.h"

// TODO: remove this dependency
#include "testing_ast.h"

using namespace sru;
using namespace std;
using namespace sru_test;

namespace sru_parser {

DEFINE_SRU_PROC(spc){
  assert(args.size() > 2);
  string str = SRUString::GetValue(args[1]);
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
  ret->Set("status", Library::Instance()->True());
  ret->Set("pos", SRUNumeric::New(epos));
  return ret;
}

DEFINE_SRU_PROC(number){
  assert(args.size() > 2);
  string str = SRUString::GetValue(args[1]);
  int pos = SRUNumeric::GetValue(args[2]);
  int epos = pos;
  while(true){
    if(epos > (int)str.size()) break;
    if(str[epos] < '0' || '9' < str[epos]) break;
    epos++;
  }
  BasicObjectPtr ret = BasicObject::New();
  if(pos == epos){
    ret->Set("status", Library::Instance()->False());
  }else{
    string substr = str.substr(pos, epos-pos);
    ret->Set("status", Library::Instance()->True());
    ret->Set("ast", C(R(R("Numeric"),"parse"),R("Numeric"),S(substr)));
    ret->Set("pos", SRUNumeric::New(epos));
  }
  return ret;
}

DEFINE_SRU_PROC(id){
  LOG << "checkid";
  assert(args.size() > 2);
  string str = SRUString::GetValue(args[1]);
  int pos = SRUNumeric::GetValue(args[2]);
  int epos = pos;
  while(true){
    if(epos > (int)str.size()) break;
    if(
        (str[epos] < 'a' || 'z' < str[epos]) &&
        (str[epos] < 'A' || 'Z' < str[epos]) &&
        (str[epos] < '0' || '9' < str[epos]) &&
        (str[epos] != '_') &&
        true
        ) break;
    epos++;
  }
  BasicObjectPtr ret = BasicObject::New();
  if(pos == epos){
    ret->Set("status", Library::Instance()->False());
  }else{
    string substr = str.substr(pos, epos-pos);
    LOG << "Match to id: " << substr;
    ret->Set("status", Library::Instance()->True());
    ret->Set("ast", R(substr));
    ret->Set("pos", SRUNumeric::New(epos));
  }
  return ret;
}

DEFINE_SRU_PROC(stringliteral){
  LOG << "checkstringliteral";
  assert(args.size() > 2);
  string str = SRUString::GetValue(args[1]);
  int pos = SRUNumeric::GetValue(args[2]);
  BasicObjectPtr ret = BasicObject::New();
  if(str[pos] != '"'){  // this is not string literal.
    ret->Set("status", Library::Instance()->False());
    return ret;
  }
  int epos = pos + 1;
  while(true){
    if(epos > (int)str.size()) break;
    if(str[epos] == '"') break;
    epos++;
  }
  if(str[epos] != '"'){ // This is't complete string literal
    ret->Set("status", Library::Instance()->False());
  }else{
    string substr = str.substr(pos+1, epos-pos-1); // cut " s
    LOG << "Match to string: " << substr;
    ret->Set("status", Library::Instance()->True());
    ret->Set("ast", S(substr));
    ret->Set("pos", SRUNumeric::New(epos + 1));
  }
  return ret;
}

}  // namespace sru_parser
