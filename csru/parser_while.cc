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


DEFINE_SRU_PROC(while_statement_end){ // this, src, pos, statement, statements
  assert(args.size() >= 5);
  LOG << "while_statement_end";
  LOG_TRACE << args[3]->Inspect();
  LOG_TRACE << args[4]->Inspect();

  return CreateTrue(
      args[2],
      CreateAst(args[1],args[2],
                P(args[3]->Get(sym::ast())),
                sym::whileTrue(),
                P(args[4]->Get(sym::ast()))
                )
      );
}


}  // namespace sru_parser

