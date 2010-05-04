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

DEFINE_SRU_PROC(let_statement_end){ // this, src, pos, flow_statement, statement
  assert(args.size() >= 5);
  LOG << "let_statement_end";
  RefExpression* ref = args[3]->Get(sym::ast())->GetData<RefExpression>();
  if(!ref) {
    return CreateFalse(args[3]->Get(sym::pos()),
                       "Let need RefExpression in left side.");
  }
  return CreateTrue(args[2],
                    L(ref->Env(),
                      ref->Name(),
                      args[4]->Get(sym::ast())));
}

}  // namespace sru_parser