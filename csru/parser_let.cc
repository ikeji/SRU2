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

DEFINE_SRU_PROC(let_statement_end){ // this, src, pos, flow_statement, statement
  PARSER_CHECK(args.size() >= 5, args[2], "Internal parser error.");
  LOG << "let_statement_end";
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  RefExpression* ref = args[3]->Get(sym::ast())->GetData<RefExpression>();
  PARSER_CHECK(ref, args[2],
      "Let need RefExpression in left side.");
  PARSER_CHECK(args[4]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  return CreateTrue(args[2],
                    L(ref->Env(),
                      ref->Name(),
                      args[4]->Get(sym::ast())));
}

}  // namespace sru_parser
