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

DEFINE_SRU_PROC(instance_method_implicit_self){ // this, src, pos, ident
  assert(args.size() >= 4);
  LOG << "instance_method_implicit_self";
  RefExpression* ref = args[3]->Get(sym::ast())->GetData<RefExpression>();
  CHECK(ref) << "instance_method_implicit_self needs RefExpressiion";
  ref->SetEnv(R(sym::self()));
  return args[3];
}

DEFINE_SRU_PROC(instance_method_self){ // this, src, pos, instance_method_implicit_self
  assert(args.size() >= 4);
  LOG << "instance_method_self";
  return CreateTrue(args[2], args[3]->Get(sym::ast()));
}

}  // namespace sru_parser
