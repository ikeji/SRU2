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

DEFINE_SRU_PROC(if_main_cond){ // this, src, pos, statement
  PARGCHK();
  PARSER_CHECK(args.size() >= 4, args[2], "Internal parser error.");
  LOG << "if_main_cond";
  LOG << args[3]->Inspect();
  
  return args[3];
}

DEFINE_SRU_PROC(if_main_then){ // this, src, pos, statements
  PARGCHK();
  PARSER_CHECK(args.size() >= 4, args[2], "Internal parser error.");
  LOG << "if_main_then";
  LOG << args[3]->Inspect();

  return args[3];
}

DEFINE_SRU_PROC(if_main_elsif){ // this, src, pos, if_main_cond, if_main_then, if_main
  PARGCHK();
  PARSER_CHECK(args.size() >= 6, args[2], "Internal parser error.");
  LOG << "if_main_else";

  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  PARSER_CHECK(args[4]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  PARSER_CHECK(args[5]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  return CreateTrue(args[2],
                    CreateAst(args[1], args[2],
                              args[3]->Get(sym::ast()),
                              sym::ifTrueFalse(),
                              args[4]->Get(sym::ast()),
                              P(args[5]->Get(sym::ast()))
                             )
                   );
}

DEFINE_SRU_PROC(if_main_else){ // this, src, pos, if_main_cond, if_main_then, statements
  PARGCHK();
  PARSER_CHECK(args.size() >= 6, args[2], "Internal parser error.");
  LOG << "if_main_else";

  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  PARSER_CHECK(args[4]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  PARSER_CHECK(args[5]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  return CreateTrue(args[2],
                    CreateAst(args[1], args[2],
                              args[3]->Get(sym::ast()),
                              sym::ifTrueFalse(),
                              args[4]->Get(sym::ast()),
                              args[5]->Get(sym::ast())
                             )
                   );
}

DEFINE_SRU_PROC(if_main_end){ // this, src, pos, if_main_cond, if_main_then
  PARGCHK();
  PARSER_CHECK(args.size() >= 5, args[2], "Internal parser error.");
  LOG << "if_main_end";

  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  PARSER_CHECK(args[4]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  return CreateTrue(args[2],
                    CreateAst(args[1], args[2],
                              args[3]->Get(sym::ast()),
                              sym::ifTrue(),
                              args[4]->Get(sym::ast())
                             )
                   );
}

}  // namespace sru_parser
