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
  return CreateTrue(epos, E(args[1],args[2],R(symbol(substr))));
}

DEFINE_SRU_PROC(const_string){
  LOG << "checkstringliteral";
  PARGCHK();
  const string& str = SRUString::GetValue(args[1]).to_str();
  int pos = SRUNumeric::GetIntValue(args[2]);
  if(str[pos] != '"'){  // this is not string literal.
    return CreateFalse(args[2], "String not found.");
  }
  int epos = pos + 1;
  while(true){
    if(epos > (int)str.size()) break;
    // No border check required becausse the '"' is exists at least.
    if(str[epos] == '"' && str[epos-1] != '\\') break;
    epos++;
  }
  if(str[epos] != '"'){ // This isn't complete string literal
    return CreateFalse(epos, "Doesn't close string.");
  }
  string substr = str.substr(pos+1, epos-pos-1); // cut " s
  string out = SRUString::UnEscapeString(substr);
  LOG << "Match to string: " << substr;
  return CreateTrue(epos + 1, E(args[1],args[2],S(symbol(out))));
}

DEFINE_SRU_PROC(number){
  PARGCHK();
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
        S(symbol(substr))
      )));
}

DEFINE_SRU_PROC(real){
  PARGCHK();
  const string& str = SRUString::GetValue(args[1]).to_str();
  int pos = SRUNumeric::GetIntValue(args[2]);
  int epos = pos;
  bool period = false;
  while(true){
    if(epos > (int)str.size()) break;
    if(str[epos] == '.'){
      if(period){
        // This is second period.
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
        S(symbol(substr))
      )));
}

DEFINE_SRU_PROC(array_literal_begin){ // this, src, pos,
  PARGCHK();
  LOG << "array_literal_begin";
  const BasicObjectPtr ast = P(
      L(sym::doldol(),
        CreateAst(args[1],args[2],
          R(sym::Array()),
          sym::mew())));
  return CreateTrue(args[2], ast);
}

DEFINE_SRU_PROC(array_literal_item){ // this, src, pos, array_literal_begin, expression
  PARGCHK();
  PARGNCHK(5);
  LOG << "array_literal_item";
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  ProcExpression* p = args[3]->Get(sym::ast())->GetData<ProcExpression>();
  PARSER_CHECK(p, args[2],
      "Array literal item need ProcExpression.");
  PARSER_CHECK(args[4]->HasSlot(sym::ast()), args[2],
      "Array literal item need expression.");
  BasicObjectPtr ast = args[4]->Get(sym::ast());
  p->Expressions()->push_back(
      C(R(R(sym::doldol()),sym::push()),
        R(sym::doldol()),
        ast).get());
  return CreateTrue(args[2], args[3]);
}

DEFINE_SRU_PROC(array_literal_end){ // this, src, pos, array_literal_begin
  PARGCHK();
  PARGNCHK(4);
  LOG << "array_literal_end";
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  BasicObjectPtr ast = args[3]->Get(sym::ast());
  ProcExpression* p = ast->GetData<ProcExpression>();
  PARSER_CHECK(p, args[2],
      "Array literal end need ProcExpression.");
  p->Expressions()->push_back(R(sym::doldol()).get());
  return CreateTrue(args[2], C(ast));
}


}  // namespace sru_parser
