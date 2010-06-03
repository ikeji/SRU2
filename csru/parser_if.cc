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

DEFINE_SRU_PROC(if_main_cond){ // this, src, pos, statement
  assert(args.size() >= 4);
  LOG << "if_main_cond";
  LOG << args[3]->Inspect();
  
  return args[3];
}

DEFINE_SRU_PROC(if_main_then){ // this, src, pos, statements
  assert(args.size() >= 4);
  LOG << "if_main_then";
  LOG << args[3]->Inspect();

  return args[3];
}

DEFINE_SRU_PROC(if_main_elsif){ // this, src, pos, if_main_cond, if_main_then, if_main
  assert(args.size() >= 6);
  LOG << "if_main_else";

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
  assert(args.size() >= 6);
  LOG << "if_main_else";

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
  assert(args.size() >= 5);
  LOG << "if_main_end";

  return CreateTrue(args[2],
                    CreateAst(args[1], args[2],
                              args[3]->Get(sym::ast()),
                              sym::ifTrue(),
                              args[4]->Get(sym::ast())
                             )
                   );
}

}  // namespace sru_parser
