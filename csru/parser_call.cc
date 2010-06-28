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

DEFINE_SRU_PROC(method_call_primary){ // this, src, pos, primary
  PARSER_CHECK(args.size() >= 4, args[2], "Internal parser error.");
  LOG << "method_call_primary";
  return args[3];
}

DEFINE_SRU_PROC(method_call_method_begin){ // this, src, pos, primary
  PARSER_CHECK(args.size() >= 3, args[2], "Internal parser error.");
  LOG << "method_call_method_begin";
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  const BasicObjectPtr pri = args[3]->Get(sym::ast());
  return CreateTrue(args[2], E(args[1],args[2], C(pri)));
}

DEFINE_SRU_PROC(method_call_method_arg){ // this, src, pos, method_call_method_begin, statement
  PARSER_CHECK(args.size() >= 5, args[2], "Internal parser error.");
  LOG << "method_call_method_arg";
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  const BasicObjectPtr c = args[3]->Get(sym::ast());
  CallExpression* call = c->GetData<CallExpression>();
  PARSER_CHECK(call, args[2], "method_call_method_arg needs call expression");
  PARSER_CHECK(args[4]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  call->Arg()->push_back(args[4]->Get(sym::ast()).get());
  return CreateTrue(args[2], c);
}

DEFINE_SRU_PROC(method_call_method_end){ // this, src, pos, method_call_primary, method_call_method_begin
  PARSER_CHECK(args.size() >= 5, args[2], "Internal parser error.");
  LOG << "method_call_method_end";
  PARSER_CHECK(args[4]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  args[3]->Set(sym::ast(), args[4]->Get(sym::ast()));
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  return CreateTrue(args[2], args[3]->Get(sym::ast()));
}

DEFINE_SRU_PROC(method_call_method_index){ // this, src, pos, method_call_primary, statement
  PARSER_CHECK(args.size() >= 5, args[2], "Internal parser error.");
  LOG << "method_call_method_index";
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  PARSER_CHECK(args[4]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  const BasicObjectPtr ast =
    CreateAst(args[1], args[2],
              args[3]->Get(sym::ast()), sym::invert(),
              args[4]->Get(sym::ast()));
  args[3]->Set(sym::ast(), ast);
  return CreateTrue(args[2], ast);
}

DEFINE_SRU_PROC(method_call_end){ // this, src, pos, method_call_primary
  PARSER_CHECK(args.size() >= 4, args[2], "Internal parser error.");
  LOG << "method_call_end";
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  return CreateTrue(args[2], args[3]->Get(sym::ast()));
}

DEFINE_SRU_PROC(primary_minus){ // this, src, pos, primary
  PARSER_CHECK(args.size() >= 4, args[2], "Internal parser error.");
  LOG << "primary_minus";
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  return CreateTrue(args[2],
                    CreateAst(args[1], args[2],
                              args[3]->Get(sym::ast()), sym::invert()));
}

DEFINE_SRU_PROC(parent){ // this, src, pos, statement
  PARSER_CHECK(args.size() >= 4, args[2], "Internal parser error.");
  LOG << "parent";
  LOG << args[2]->Inspect();
  LOG << args[3]->Inspect();
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  return CreateTrue(args[2], args[3]->Get(sym::ast()));
}

}  // namespace sru_parser
