// Programing Language SRU
// Copyright(C) 2005-2010 IKeJI
// 

#include <string>
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
  PARGCHK();
  PARSER_CHECK(args.size() >= 4, args[2], "Internal parser error.");
  LOG << "def_statement_begin";
  LOG << args[3]->Inspect();
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  RefExpression* ref = args[3]->Get(sym::ast())->GetData<RefExpression>();
  PARSER_CHECK(ref, args[2], "def statement need RefExpression for name.");
  const BasicObjectPtr p = P();
  p->GetData<ProcExpression>()->SetRetVal(sym::leturn());
  const BasicObjectPtr ret = CreateTrue(args[2], L(ref->Env(),
                                                   ref->Name(),
                                                   p));
  ret->Set(sym::doldol(), p);
  return ret;
}

DEFINE_SRU_PROC(def_statement_varg){ // this, src, pos, def_statement_begin, ident
  PARGCHK();
  PARSER_CHECK(args.size() >= 5, args[2], "Internal parser error.");
  LOG << "def_statement_varg";
  PARSER_CHECK(args[3]->HasSlot(sym::doldol()), args[2], "Internal parser error.");
  ProcExpression* p = args[3]->Get(sym::doldol())->GetData<ProcExpression>();
  PARSER_CHECK(p, args[2], "def statement varg need ProcExpression.");
  PARSER_CHECK(args[4]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  RefExpression* ref = args[4]->Get(sym::ast())->GetData<RefExpression>();
  PARSER_CHECK(ref, args[2], "def statement varg need RefExpression.");
  p->Varg()->push_back(SRUString::GetValue(ref->Name()));
  return args[4];
}

DEFINE_SRU_PROC(def_statement_end){ // this, src, pos, def_statement_begin, statements
  PARGCHK();
  PARSER_CHECK(args.size() >= 5, args[2], "Internal parser error.");
  LOG << "def_statement_end";
  PARSER_CHECK(args[3]->HasSlot(sym::doldol()), args[2], "Internal parser error.");
  ProcExpression* p = args[3]->Get(sym::doldol())->GetData<ProcExpression>();
  PARSER_CHECK(p, args[2], 
      "def statement end need ProcExpression.");
  PARSER_CHECK(args[4]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  ProcExpression* s = args[4]->Get(sym::ast())->GetData<ProcExpression>();
  PARSER_CHECK(s, args[2],
      "def statement end need ProcExpression as statement.");
  
  for (object_vector::iterator it = s->Expressions()->begin();
       it != s->Expressions()->end();
       it++) {
    p->Expressions()->push_back(*it);
  }

  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  return CreateTrue(args[2], args[3]->Get(sym::ast()));
}

}  // namespace sru_parser
