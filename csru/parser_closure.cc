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

DEFINE_SRU_PROC(closure_begin){ // this, src, pos, 
  PARGCHK();
  LOG << "closure_begin";
  return CreateTrue(args[2], E(args[1], args[2], P()));
}

DEFINE_SRU_PROC(closure_merge_varg){ // this, src, pos, closure_begin, closure_varg
  PARGCHK();
  PARGNCHK(5);
  LOG << "closure_merge_varg";
  LOG << "Merge: " << args[3]->Inspect() << " and " << args[4]->Inspect();
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  ProcExpression* p = args[3]->Get(sym::ast())->GetData<ProcExpression>();
  PARSER_CHECK(p, args[2], "Need proc for closure_statement");
  PARSER_CHECK(args[4]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  ProcExpression* p2 = args[4]->Get(sym::ast())->GetData<ProcExpression>();
  PARSER_CHECK(p2, args[2], "Need two proc for closure_statement");
  p->SetVarg(*p2->Varg());
  p->SetRetVal(p2->RetVal());
  p->SetExpressions(*p2->Expressions());
  return args[4];
}

DEFINE_SRU_PROC(closure_statements){ // this, src, pos, closure_begin, statements
  PARGCHK();
  PARGNCHK(5);
  LOG << "closure_statement";
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  ProcExpression* p = args[3]->Get(sym::ast())->GetData<ProcExpression>();
  PARSER_CHECK(p, args[2], "Need proc for closure_statement");
  PARSER_CHECK(args[4]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  ProcExpression* s = args[4]->Get(sym::ast())->GetData<ProcExpression>();
  PARSER_CHECK(s, args[2], "Need proc as statements for closure_statement");

  for (object_vector::iterator it = s->Expressions()->begin();
      it != s->Expressions()->end();
      it++) {
    p->Expressions()->push_back(*it);
  }

  return args[4];
}

DEFINE_SRU_PROC(closure_end){ // this, src, pos, closure_begin
  PARGCHK();
  PARGNCHK(4);
  LOG << "closure_end";
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  return CreateTrue(args[2], args[3]->Get(sym::ast()));
}

DEFINE_SRU_PROC(closure_varg_begin){ // this, src, pos, 
  PARGCHK();
  LOG << "closure_varg_begin";
  return CreateTrue(args[2], P());
}

DEFINE_SRU_PROC(closure_varg_idents){ // this, src, pos, closure_varg_begin, ident
  PARGCHK();
  PARGNCHK(5);
  LOG << "closure_varg_idents";
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  ProcExpression* p = args[3]->Get(sym::ast())->GetData<ProcExpression>();
  PARSER_CHECK(p, args[2], "Need proc for closure_varg_idents");
  PARSER_CHECK(args[4]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  RefExpression* ref = args[4]->Get(sym::ast())->GetData<RefExpression>();
  PARSER_CHECK(ref, args[2], "Need RefExpression for closure_varg_idents");
  p->Varg()->push_back(SRUString::GetValue(ref->Name()));
  return args[4];
}

DEFINE_SRU_PROC(closure_varg_retarg){ // this, src, pos, closure_varg_begin, ident
  PARGCHK();
  PARGNCHK(5);
  LOG << "closure_varg_retarg";
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  ProcExpression* p = args[3]->Get(sym::ast())->GetData<ProcExpression>();
  PARSER_CHECK(p, args[2], "Need proc for closure_varg_retarg");
  PARSER_CHECK(args[4]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  RefExpression* ref = args[4]->Get(sym::ast())->GetData<RefExpression>();
  PARSER_CHECK(ref, args[2],  "Need RefExpression for closure_varg_retarg");
  p->SetRetVal(SRUString::GetValue(ref->Name()));
  return args[4];
}

DEFINE_SRU_PROC(closure_varg_end){ // this, src, pos, closure_varg_begin
  PARGCHK();
  PARGNCHK(4);
  LOG << "closure_varg_end";
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  return CreateTrue(args[2], args[3]->Get(sym::ast()));
}

}  // namespace sru_parser

