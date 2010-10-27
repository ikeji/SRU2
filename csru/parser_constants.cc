// Programing Language SRU
// Copyright(C) 2005-2010 IKeJI
//

#include <string>
#include "native_proc.h"
#include "basic_object.h"
#include "constants.h"
#include "sru_string.h"
#include "numeric.h"
#include "parser_util.h"

// TODO: remove this dependency
#include "testing_ast.h"

using namespace sru;
using namespace std;
using namespace sru_test;

namespace sru_parser {

DEFINE_SRU_PROC(ident){
  LOG << "checkid";
  PARGCHK();
  PARSER_CHECK(args.size() >= 3, args[2], "Internal parser error.");
  const string& str = SRUString::GetValue(args[1]).to_str();
  int pos = SRUNumeric::GetIntValue(args[2]);
  int epos = pos;
  while(true){
    if(epos > (int)str.size()) break;
    // Look ahead for cut before "!=".
    if(epos + 1 < (int)str.size() &&
       str[epos] == '!' &&
       str[epos + 1] == '=') break;
    if(
        (str[epos] < 'a' || 'z' < str[epos]) &&
        (str[epos] < 'A' || 'Z' < str[epos]) &&
        ( epos == pos ||
          (str[epos] < '0' || '9' < str[epos])
        ) &&
        (str[epos] != '_') &&
        (str[epos] != '!') &&
        // TODO: Should I enable this?
        //(str[epos] != '?') &&
        true
        ) break;
    epos++;
  }
  if(pos == epos){
    return CreateFalse(args[2], "ID not found.");
  }
  string substr = str.substr(pos, epos-pos);
  LOG << "Match to id: " << substr;
  return CreateTrue(epos, E(args[1],args[2],R(symbol(substr.c_str()))));
}

DEFINE_SRU_PROC(const_string){
  LOG << "checkstringliteral";
  PARGCHK();
  PARSER_CHECK(args.size() >= 3, args[2], "Internal parser error.");
  const string& str = SRUString::GetValue(args[1]).to_str();
  int pos = SRUNumeric::GetIntValue(args[2]);
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
  return CreateTrue(epos + 1, E(args[1],args[2],S(symbol(substr.c_str()))));
}

DEFINE_SRU_PROC(number){
  PARGCHK();
  PARSER_CHECK(args.size() >= 3, args[2], "Internal parser error.");
  const string& str = SRUString::GetValue(args[1]).to_str();
  int pos = SRUNumeric::GetIntValue(args[2]);
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
  return CreateTrue(epos,E(args[1],args[2],
      C(R(R(sym::Numeric()),sym::parse()),
        R(sym::Numeric()),
        S(symbol(substr.c_str()))
      )));
}

DEFINE_SRU_PROC(real){
  PARGCHK();
  PARSER_CHECK(args.size() >= 3, args[2], "Internal parser error.");
  const string& str = SRUString::GetValue(args[1]).to_str();
  int pos = SRUNumeric::GetIntValue(args[2]);
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
  return CreateTrue(epos,E(args[1],args[2],
      C(R(R(sym::Numeric()),sym::parse()),
        R(sym::Numeric()),
        S(symbol(substr.c_str()))
      )));
}

}  // namespace sru_parser
