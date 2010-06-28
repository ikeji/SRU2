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
  PARSER_CHECK(args.size() >= 3, args[2], "Internal parser error.");
  LOG << "statements_begin";
  return CreateTrue(args[2], E(args[1],args[2], P()));
}

DEFINE_SRU_PROC(statements_statement){ // this, src, pos, statements_begin, statement
  PARSER_CHECK(args.size() >= 5, args[2], "Internal parser error.");
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
  PARSER_CHECK(args.size() >= 4, args[2], "Internal parser error.");
  LOG << "statements_end";
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  return CreateTrue(args[2], args[3]->Get(sym::ast()));
}

}  // namespace sru_parser
