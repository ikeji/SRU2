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

DEFINE_SRU_PROC(statements_begin){ // this, src, pos, 
  PARGCHK();
  LOG << "statements_begin";
  return CreateTrue(args[2], E(args[1],args[2], P()));
}

DEFINE_SRU_PROC(statements_statement){ // this, src, pos, statements_begin, statement
  PARGCHK();
  PARGNCHK(5);
  LOG << "statements_statement";
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  ProcExpression* p = args[3]->Get(sym::ast())->GetData<ProcExpression>();
  PARSER_CHECK(p, args[2], "Need proc for statements_statement");
  PARSER_CHECK(args[4]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  const BasicObjectPtr prog = args[4]->Get(sym::ast());
  p->Expressions()->push_back(prog.get());
  return args[4];
}

DEFINE_SRU_PROC(statements_end){ // this, src, pos, statements_begin
  PARGCHK();
  PARGNCHK(4);
  LOG << "statements_end";
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  return CreateTrue(args[2], args[3]->Get(sym::ast()));
}

DEFINE_SRU_PROC(statement_begin){ // this, src, pos, 
  PARGCHK();
  LOG << "statement_begin";
  return CreateTrue(args[2], Library::Instance()->Nil());
}

DEFINE_SRU_PROC(statement_main){ // this, src, pos, statement_begin, let_statement
  PARGCHK();
  PARGNCHK(5);
  LOG << "statement_main";
  PARSER_CHECK(args[4]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  const BasicObjectPtr ast = args[4]->Get(sym::ast());
  args[3]->Set(sym::ast(), ast);
  return CreateTrue(args[2], ast);
}

DEFINE_SRU_PROC(statement_if){ // this, src, pos, statement_begin, statement
  PARGCHK();
  PARGNCHK(5);
  LOG << "statement_if";
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  PARSER_CHECK(args[4]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  const BasicObjectPtr ast = args[3]->Get(sym::ast());
  const BasicObjectPtr cond = args[4]->Get(sym::ast());
  const BasicObjectPtr new_ast = 
    CreateAst(args[1],args[2],
        cond,
        sym::ifTrue(),
        P(ast));
  args[3]->Set(sym::ast(), new_ast);
  return CreateTrue(args[2], new_ast);
}

DEFINE_SRU_PROC(statement_end){ // this, src, pos, statement_begin
  PARGCHK();
  PARGNCHK(4);
  LOG << "statement_end";
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  return CreateTrue(args[2], args[3]->Get(sym::ast()));
}

}  // namespace sru_parser
