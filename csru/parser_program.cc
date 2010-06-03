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

DEFINE_SRU_PROC(program_end){ // this, src, pos, statement
  assert(args.size() >= 4);
  LOG << "program_end";
  return CreateTrue(args[2], E(args[1], args[2], args[3]->Get(sym::ast())));
}

}  // namespace sru_parser

