// Programing Language SRU
// Copyright(C) 2005-2010 IKeJI
// 

#include <string>
#include "native_proc.h"
#include "basic_object.h"
#include "constants.h"
#include "library.h"
#include "string.h"
#include "numeric.h"

// TODO: remove this dependency
#include "testing_ast.h"

using namespace sru;
using namespace std;
using namespace sru_test;

BasicObjectPtr CreateFalse(BasicObjectPtr pos, BasicObjectPtr error_message){
  BasicObjectPtr ret = BasicObject::New();
  ret->Set(sym::status(), Library::Instance()->False());
  ret->Set(sym::pos(), pos);
  ret->Set(sym::error(), error_message);
  return ret;
}

BasicObjectPtr CreateFalse(BasicObjectPtr pos, const char* error_message){
  return CreateFalse(pos, SRUString::New(symbol(error_message)));
}

BasicObjectPtr CreateFalse(int pos, const char* error_message){
  return CreateFalse(SRUNumeric::New(pos),
      SRUString::New(symbol(error_message)));
}

BasicObjectPtr CreateTrue(BasicObjectPtr pos, BasicObjectPtr ast){
  BasicObjectPtr ret = BasicObject::New();
  ret->Set(sym::status(), Library::Instance()->True());
  ret->Set(sym::pos(), pos);
  ret->Set(sym::ast(), ast);
  return ret;
}

BasicObjectPtr CreateTrue(int pos, BasicObjectPtr ast){
  return CreateTrue(SRUNumeric::New(pos), ast);
}

namespace sru_parser {

DEFINE_SRU_PROC(ident){
  LOG << "checkid";
  assert(args.size() > 2);
  const string& str = SRUString::GetValue(args[1]).to_str();
  int pos = SRUNumeric::GetValue(args[2]);
  int epos = pos;
  while(true){
    if(epos > (int)str.size()) break;
    if(
        (str[epos] < 'a' || 'z' < str[epos]) &&
        (str[epos] < 'A' || 'Z' < str[epos]) &&
        ( epos == pos ||
          (str[epos] < '0' || '9' < str[epos])
        ) &&
        (str[epos] != '_') &&
        true
        ) break;
    epos++;
  }
  if(pos == epos){
    return CreateFalse(args[2], "ID not found.");
  }
  string substr = str.substr(pos, epos-pos);
  LOG << "Match to id: " << substr;
  return CreateTrue(epos, R(symbol(substr.c_str())));
}

DEFINE_SRU_PROC(const_string){
  LOG << "checkstringliteral";
  assert(args.size() > 2);
  const string& str = SRUString::GetValue(args[1]).to_str();
  int pos = SRUNumeric::GetValue(args[2]);
  if(str[pos] != '"'){  // this is not string literal.
    return CreateFalse(args[2], "String not found.");
  }
  int epos = pos + 1;
  while(true){
    if(epos > (int)str.size()) break;
    if(str[epos] == '"') break;
    epos++;
  }
  if(str[epos] != '"'){ // This isn't complete string literal
    return CreateFalse(epos, "Doesn't close string.");
  }
  string substr = str.substr(pos+1, epos-pos-1); // cut " s
  LOG << "Match to string: " << substr;
  return CreateTrue(epos + 1, S(symbol(substr.c_str())));
}

DEFINE_SRU_PROC(number){
  assert(args.size() > 2);
  const string& str = SRUString::GetValue(args[1]).to_str();
  int pos = SRUNumeric::GetValue(args[2]);
  int epos = pos;
  while(true){
    if(epos > (int)str.size()) break;
    if(str[epos] < '0' || '9' < str[epos]) break;
    epos++;
  }
  if(pos == epos){
    return CreateFalse(args[2], "Number not found.");
  }
  string substr = str.substr(pos, epos-pos);
  return CreateTrue(epos,
      C(R(R(sym::Numeric()),sym::parse()),
        R(sym::Numeric()),
        S(symbol(substr.c_str()))
      ));
}

DEFINE_SRU_PROC(real){
  assert(args.size() > 2);
  const string& str = SRUString::GetValue(args[1]).to_str();
  int pos = SRUNumeric::GetValue(args[2]);
  int epos = pos;
  bool period = false;
  while(true){
    if(epos > (int)str.size()) break;
    if(str[epos] == '.'){
      if(period){
        // This is second period.
        epos--;
        break;
      }
      period = true;
    } else if(
        (str[epos] < '0' || '9' < str[epos]) &&
        (str[epos] != '.') &&
        true
        ){
      break;
    }
    epos++;
  }
  if(pos == epos){
    return CreateFalse(args[2], "Real not found.");
  }
  if(str[epos-1] == '.') return CreateFalse(epos-1, "Real couldn't end with period.");
  string substr = str.substr(pos, epos-pos);
  return CreateTrue(epos,
      C(R(R(sym::Numeric()),sym::parse()),
        R(sym::Numeric()),
        S(symbol(substr.c_str()))
      ));
}

}  // namespace sru_parser