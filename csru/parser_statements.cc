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

DEFINE_SRU_PROC(statements_begin){ // this, src, pos, 
  assert(args.size() >= 3);
  LOG << "statements_begin";
  return CreateTrue(args[2], E(args[1],args[2], P()));
}

DEFINE_SRU_PROC(statements_statement){ // this, src, pos, statements_begin, statement
  assert(args.size() >= 5);
  LOG << "statements_statement";
  ProcExpression* p = args[3]->Get(sym::ast())->GetData<ProcExpression>();
  CHECK(p) << "Need proc for statements_statement";
  BasicObjectPtr prog = args[4]->Get(sym::ast());
  p->Expressions()->push_back(prog.get());
  return args[4];
}

DEFINE_SRU_PROC(statements_end){ // this, src, pos, statements_begin
  assert(args.size() >= 4);
  LOG << "statements_end";
  return CreateTrue(args[2], args[3]->Get(sym::ast()));
}

}  // namespace sru_parser
