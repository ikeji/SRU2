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

DEFINE_SRU_PROC(method_call_primary){ // this, src, pos, primary
  assert(args.size() >= 4);
  LOG << "method_call_primary";
  return args[3];
}

DEFINE_SRU_PROC(method_call_method_begin){ // this, src, pos, primary
  assert(args.size() >= 3);
  LOG << "method_call_method_begin";
  BasicObjectPtr pri = args[3]->Get(sym::ast());
  return CreateTrue(args[2], E(args[1],args[2], C(pri)));
}

DEFINE_SRU_PROC(method_call_method_arg){ // this, src, pos, method_call_method_begin, statement
  assert(args.size() >= 5);
  LOG << "method_call_method_arg";
  BasicObjectPtr c = args[3]->Get(sym::ast());
  CallExpression* call = c->GetData<CallExpression>();
  PARSER_CHECK(call, args[2], "method_call_method_arg needs call expression");
  call->Arg()->push_back(args[4]->Get(sym::ast()).get());
  return CreateTrue(args[2], c);
}

DEFINE_SRU_PROC(method_call_method_end){ // this, src, pos, method_call_primary, method_call_method_begin
  assert(args.size() >= 5);
  LOG << "method_call_method_end";
  args[3]->Set(sym::ast(), args[4]->Get(sym::ast()));
  return CreateTrue(args[2], args[3]->Get(sym::ast()));
}

DEFINE_SRU_PROC(method_call_method_index){ // this, src, pos, method_call_primary, statement
  assert(args.size() >= 5);
  LOG << "method_call_method_index";
  BasicObjectPtr ast = 
    CreateAst(args[1], args[2],
              args[3]->Get(sym::ast()), sym::invert(),
              args[4]->Get(sym::ast()));
  args[3]->Set(sym::ast(), ast);
  return CreateTrue(args[2], ast);
}

DEFINE_SRU_PROC(method_call_end){ // this, src, pos, method_call_primary
  assert(args.size() >= 4);
  LOG << "method_call_end";
  return CreateTrue(args[2], args[3]->Get(sym::ast()));
}

DEFINE_SRU_PROC(primary_minus){ // this, src, pos, primary
  assert(args.size() >= 4);
  LOG << "primary_minus";
  return CreateTrue(args[2],
                    CreateAst(args[1], args[2],
                              args[3]->Get(sym::ast()), sym::invert()));
}

DEFINE_SRU_PROC(parent){ // this, src, pos, statement
  assert(args.size() >= 4);
  LOG << "parent";
  LOG << args[2]->Inspect();
  LOG << args[3]->Inspect();
  return CreateTrue(args[2], args[3]->Get(sym::ast()));
}

}  // namespace sru_parser
