// Programing Language SRU
// Copyright(C) 2005-2010 IKeJI
// 

#include <string>
#include "native_proc.h"
#include "string.h"
#include "numeric.h"
#include "library.h"
#include "logging.h"
#include "ast.h"
#include "constants.h"

// TODO: remove this dependency
#include "testing_ast.h"

using namespace sru;
using namespace std;
using namespace sru_test;

namespace sru_parser {

// TODO: Write unit test for each proc in this file.

BasicObjectPtr CreateResult(const BasicObjectPtr& status,
                            const BasicObjectPtr& ast,
                            const BasicObjectPtr& pos){
  BasicObjectPtr ret = BasicObject::New();
  ret->Set(sym::status(), status);
  ret->Set(sym::ast(), ast);
  ret->Set(sym::pos(), pos);
  return ret;
}
BasicObjectPtr CreateResult(bool status,
                            const BasicObjectPtr& ast,
                            const BasicObjectPtr& pos){
  return CreateResult(
      status ? Library::Instance()->True() : Library::Instance()->False(),
      ast,
      pos);
}
BasicObjectPtr CreateResult(const BasicObjectPtr& status,
                            const BasicObjectPtr& ast,
                            int pos){
  return CreateResult(status, ast, SRUNumeric::New(pos));
}

BasicObjectPtr CreateResult(bool status,
                            const BasicObjectPtr& ast,
                            int pos){
  return CreateResult(status, ast, SRUNumeric::New(pos));
}

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

DEFINE_SRU_PROC(id){
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
  BasicObjectPtr ret = BasicObject::New();
  if(pos == epos){
    ret->Set(sym::status(), Library::Instance()->False());
    ret->Set(sym::pos(), args[2]);
    ret->Set(sym::error(), SRUString::New(symbol("ID not found.")));
  }else{
    string substr = str.substr(pos, epos-pos);
    LOG << "Match to id: " << substr;
    ret->Set(sym::status(), Library::Instance()->True());
    ret->Set(sym::ast(), R(symbol(substr.c_str())));
    ret->Set(sym::pos(), SRUNumeric::New(epos));
  }
  return ret;
}

DEFINE_SRU_PROC(stringliteral){
  LOG << "checkstringliteral";
  assert(args.size() > 2);
  const string& str = SRUString::GetValue(args[1]).to_str();
  int pos = SRUNumeric::GetValue(args[2]);
  BasicObjectPtr ret = BasicObject::New();
  if(str[pos] != '"'){  // this is not string literal.
    ret->Set(sym::status(), Library::Instance()->False());
    ret->Set(sym::pos(), args[2]);
    ret->Set(sym::error(), SRUString::New(symbol("String not found.")));
    return ret;
  }
  int epos = pos + 1;
  while(true){
    if(epos > (int)str.size()) break;
    if(str[epos] == '"') break;
    epos++;
  }
  if(str[epos] != '"'){ // This is't complete string literal
    ret->Set(sym::status(), Library::Instance()->False());
    ret->Set(sym::pos(), SRUNumeric::New(epos));
    ret->Set(sym::error(), SRUString::New(symbol("Don't close string.")));
  }else{
    string substr = str.substr(pos+1, epos-pos-1); // cut " s
    LOG << "Match to string: " << substr;
    ret->Set(sym::status(), Library::Instance()->True());
    ret->Set(sym::ast(), S(symbol(substr.c_str())));
    ret->Set(sym::pos(), SRUNumeric::New(epos + 1));
  }
  return ret;
}

DEFINE_SRU_PROC(numericliteral){
  assert(args.size() > 2);
  const string& str = SRUString::GetValue(args[1]).to_str();
  int pos = SRUNumeric::GetValue(args[2]);
  int epos = pos;
  while(true){
    if(epos > (int)str.size()) break;
    if(str[epos] < '0' || '9' < str[epos]) break;
    epos++;
  }
  BasicObjectPtr ret = BasicObject::New();
  if(pos == epos){
    ret->Set(sym::status(), Library::Instance()->False());
    ret->Set(sym::pos(), args[2]);
    ret->Set(sym::error(), SRUString::New(symbol("number not found.")));
  }else{
    string substr = str.substr(pos, epos-pos);
    ret->Set(sym::status(), Library::Instance()->True());
    ret->Set(sym::ast(), C(R(R(sym::Numeric()),sym::parse()),R(sym::Numeric()),S(symbol(substr.c_str()))));
    ret->Set(sym::pos(), SRUNumeric::New(epos));
  }
  return ret;
}


DEFINE_SRU_PROC(prog_begin){ // this, src, pos
  assert(args.size() >= 3);
  LOG << "prog_begin";
  return CreateResult(true, P(), args[2]); // return true, empty closure, pos
}

DEFINE_SRU_PROC(prog_rep){ // this, src, pos, prog_begin, expression
  assert(args.size() >= 5);
  LOG << "prog_rep called with " << args[3]->Inspect() << ", " << args[4]->Inspect();
  ProcExpression* pr = args[3]->Get(sym::ast())->GetData<ProcExpression>();
  pr->Expressions()->push_back(args[4]->Get(sym::ast()).get());
  return CreateResult(true, args[3]->Get(sym::ast()), args[2]);
}

DEFINE_SRU_PROC(prog_end){ // this, src, pos, id, expression
  assert(args.size() >= 4);
  LOG << "prog_end";
  return CreateResult(true, C(args[3]->Get(sym::ast())), args[2]); // return true, closure, pos
}

DEFINE_SRU_PROC(def_creat){ // this, src, pos, id, expression
  assert(args.size() >= 5);
  LOG << "def_creat";
  RefExpression* ref = args[3]->Get(sym::ast())->GetData<RefExpression>();
  CHECK(ref) << "Need RefExpression for def";
  LOG << args[4]->Inspect();
  BasicObjectPtr exp = args[4]->Get(sym::ast());
  return CreateResult(true, L(ref->Env(),ref->Name(),exp), args[2]);
}

DEFINE_SRU_PROC(lamb_begin){ // this, src, pos,
  assert(args.size() >= 3);
  LOG << "lamb_begin";
  return CreateResult(true, P(),args[2]);
}

DEFINE_SRU_PROC(lamb_arg){ // this, src, pos, lamb_begin, id
  assert(args.size() >= 5);
  LOG << "lamb_arg";
  ProcExpression* p = args[3]->Get(sym::ast())->GetData<ProcExpression>();
  CHECK(p) << "Need ProcExpression for lambda";
  RefExpression* ref = args[4]->Get(sym::ast())->GetData<RefExpression>();
  CHECK(ref) << "Need RefExpression for lambda";
  p->Varg()->push_back(SRUString::GetValue(ref->Name()));
  return CreateResult(true, args[3]->Get(sym::ast()), args[2]);
}

DEFINE_SRU_PROC(lamb_end){ // this, src, pos, lamb_begin, program
  assert(args.size() >= 5);
  LOG << "lamb_end";
  ProcExpression* p = args[3]->Get(sym::ast())->GetData<ProcExpression>();
  CHECK(p) << "Need ProcExpression by lambda for lambda";
  BasicObjectPtr prog = args[4]->Get(sym::ast());
  CHECK(prog.get()) << "Need program for lambda";
  p->Expressions()->push_back(prog.get());
  return CreateResult(true, args[3]->Get(sym::ast()), args[2]);
}

DEFINE_SRU_PROC(call_begin){ // this, src, pos, expression
  assert(args.size() >= 4);
  LOG << "call_begin";
  return CreateResult(true, C(args[3]->Get(sym::ast())), args[2]);
}

DEFINE_SRU_PROC(call_rep){ // this, src, pos, call_begin, expression
  assert(args.size() >= 5);
  LOG << "call_rep";
  CallExpression* call = args[3]->Get(sym::ast())->GetData<CallExpression>();
  call->Arg()->push_back(args[4]->Get(sym::ast()).get());
  return CreateResult(true, args[3]->Get(sym::ast()), args[2]);
}

DEFINE_SRU_PROC(call_end){ // this, src, pos, call_begin
  assert(args.size() >= 4);
  LOG << "call_end";
  return CreateResult(true, args[3]->Get(sym::ast()), args[2]);
}

DEFINE_SRU_PROC(ref_ins){ // this, src, pos, id, ins
  assert(args.size() >= 5);
  LOG << "ref_ins";
  RefExpression* ref = args[4]->Get(sym::ast())->GetData<RefExpression>();
  CHECK(ref) << "Need RefExpression by ref for lambda";
  while(ref->Env() != NULL && ref->Env() != Library::Instance()->Nil()){
    ref = ref->Env()->GetData<RefExpression>();
    CHECK(ref) << "Need RefExpression chain for lambda";
  }
  ref->SetEnv(args[3]->Get(sym::ast()));
  return CreateResult(true, args[4]->Get(sym::ast()), args[2]);
}


}  // namespace sru_parser
