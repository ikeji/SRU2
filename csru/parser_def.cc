// Programing Language SRU
// Copyright(C) 2005-2010 IKeJI
// 

#include <string>
#include <cassert>
#include "native_proc.h"
#include "basic_object.h"
#include "constants.h"
#include "library.h"
#include "numeric.h"
#include "parser_util.h"

// TODO: remove this dependency
#include "testing_ast.h"

using namespace sru;
using namespace std;
using namespace sru_test;

namespace sru_parser {

DEFINE_SRU_PROC(def_statement_begin){ // this, src, pos, flow_statement
  assert(args.size() >= 4);
  LOG << "def_statement_begin";
  LOG << args[3]->Inspect();
  RefExpression* ref = args[3]->Get(sym::ast())->GetData<RefExpression>();
  if(!ref){
    return CreateFalse(args[3]->Get(sym::pos()),
                       "def statement need RefExpression for name.");
  }
  BasicObjectPtr p = P();
  p->GetData<ProcExpression>()->SetRetVal(sym::leturn());
  BasicObjectPtr ret = CreateTrue(args[2], L(ref->Env(),
                                             ref->Name(),
                                             p));
  ret->Set(sym::doldol(), p);
  return ret;
}

DEFINE_SRU_PROC(def_statement_varg){ // this, src, pos, def_statement_begin, ident
  assert(args.size() >= 5);
  LOG << "def_statement_varg";
  ProcExpression* p = args[3]->Get(sym::doldol())->GetData<ProcExpression>();
  if(!p){
    return CreateFalse(args[3]->Get(sym::pos()),
                       "def statement varg need ProcExpression.");
  }
  RefExpression* ref = args[4]->Get(sym::ast())->GetData<RefExpression>();
  if(!ref){
    return CreateFalse(args[3]->Get(sym::pos()),
                       "def statement varg need RefExpression.");
  }
  p->Varg()->push_back(SRUString::GetValue(ref->Name()));
  return args[4];
}

DEFINE_SRU_PROC(def_statement_end){ // this, src, pos, def_statement_begin, statements
  assert(args.size() >= 5);
  LOG << "def_statement_end";
  ProcExpression* p = args[3]->Get(sym::doldol())->GetData<ProcExpression>();
  if(!p){
    return CreateFalse(args[3]->Get(sym::pos()),
                       "def statement end need ProcExpression.");
  }
  p->Expressions()->push_back(args[4]->Get(sym::ast()).get());
  return CreateTrue(args[2], args[3]->Get(sym::ast()));
}

}  // namespace sru_parser
