// Programing Language SRU
// Copyright(C) 2005-2010 IKeJI
// 

#include <string>
#include <iostream>
#include "native_proc.h"
#include "string.h"
#include "numeric.h"
#include "library.h"

// TODO: remove this dependency
#include "testing_ast.h"

using namespace sru;
using namespace std;
using namespace sru_test;

namespace sru_parser {

DEFINE_SRU_PROC(spc){
  assert(arg.size() > 2);
  string str = SRUString::GetValue(arg[1]);
  int pos = SRUNumeric::GetValue(arg[2]);
  int epos = pos;
#if DEBUG
  cout << "spc start: pos = " << epos;
#endif
  while(true){
    if(epos > (int)str.size()) break;
    if(str[epos] != ' ') break;
    epos++;
  }
#if DEBUG
  cout << "spc result: epos = " << epos;
#endif
  BasicObjectPtr ret = BasicObject::New();
  ret->Set("status", Library::Instance()->True());
  ret->Set("pos", SRUNumeric::New(epos));
  return ret;
}

DEFINE_SRU_PROC(number){
  assert(arg.size() > 2);
  string str = SRUString::GetValue(arg[1]);
  int pos = SRUNumeric::GetValue(arg[2]);
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
#if DEBUG
  cout << "checkid" << endl;
#endif
  assert(arg.size() > 2);
  string str = SRUString::GetValue(arg[1]);
  int pos = SRUNumeric::GetValue(arg[2]);
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
#if DEBUG
    cout << "Match to id: " << substr << endl;
#endif
    ret->Set("status", Library::Instance()->True());
    ret->Set("ast", R(substr));
    ret->Set("pos", SRUNumeric::New(epos));
  }
  return ret;
}

DEFINE_SRU_PROC(stringliteral){
#if DEBUG
  cout << "checkstringliteral" << endl;
#endif
  assert(arg.size() > 2);
  string str = SRUString::GetValue(arg[1]);
  int pos = SRUNumeric::GetValue(arg[2]);
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
#if DEBUG
    cout << "Match to string: " << substr << endl;
#endif
    ret->Set("status", Library::Instance()->True());
    ret->Set("ast", S(substr));
    ret->Set("pos", SRUNumeric::New(epos + 1));
  }
  return ret;
}

}  // namespace sru_parser
