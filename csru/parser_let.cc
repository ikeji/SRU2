// Programing Language SRU
// Copyright(C) 2005-2010 IKeJI
// 

#include <string>
#include "native_proc.h"
#include "basic_object.h"
#include "constants.h"
#include "numeric.h"
#include "parser_util.h"
#include "utils.h"

// TODO: remove this dependency
#include "testing_ast.h"

using namespace sru;
using namespace std;
using namespace sru_test;

namespace sru_parser {

bool TryGetGetProc(
    const BasicObjectPtr left,
    BasicObjectPtr* target,
    BasicObjectPtr* index){
  LOG_TRACE << 1;
  // if left is "get" proc, it created by method_call_method_index and
  // CreateAst.
  // left == ({|$$|($$.get)($$, index)}(target))
  // c == ({|$$|($$.get)($$, index)}(target))
  CallExpression* c = left->GetData<CallExpression>();
  if(!c) return false;
  if(c->Arg()->size() != 1) return false;
  LOG_TRACE << 2;
  // p == {|$$|($$.get)($$, index)}
  ProcExpression* p = c->Proc()->GetData<ProcExpression>();
  if(!p) return false;
  if(!(p->Varg()->size() == 1 && p->Varg()->at(0) == sym::doldol())){
    return false;
  }
  if(!(p->Expressions()->size() == 1)) return false;
  LOG_TRACE << 3;
  // c2 == ($$.get)($$, index)
  CallExpression* c2 = p->Expressions()->at(0)->GetData<CallExpression>();
  if(!c2) return false;
  LOG_TRACE << 4;
  // r == $$.get
  RefExpression* r = c2->Proc()->GetData<RefExpression>();
  LOG_TRACE << c2->Proc()->Inspect();
  if(!r) return false;
  LOG_TRACE << 42;
  LOG_TRACE << r->Name()->Inspect();
  LOG_TRACE << sym::get().getid();
  LOG_TRACE << SRUString::GetValue(r->Name()).getid();
  if(SRUString::GetValue(r->Name()) != sym::get()) return false;
  LOG_TRACE << 5;
  // r2 == $$
  RefExpression* r2 = r->Env()->GetData<RefExpression>();
  if(!r2) return false;
  LOG_TRACE << 51;
  LOG_TRACE << r2->Env().get();
  if(!IsNil(r2->Env())) return false;
  LOG_TRACE << 52;
  if(SRUString::GetValue(r2->Name()) != sym::doldol()) return false;
  LOG_TRACE << 53;
  if(c2->Arg()->size() != 2) return false;
  LOG_TRACE << 6;
  // r3 == $$
  RefExpression* r3 = c2->Arg()->at(0)->GetData<RefExpression>();
  if(!r3) return false;
  if(!IsNil(r3->Env())) return false;
  if(SRUString::GetValue(r3->Name()) != sym::doldol()) return false;
 
  *target = c->Arg()->at(0);
  *index = c2->Arg()->at(1);
  return true;
}

DEFINE_SRU_PROC(let_statement_end){ // this, src, pos, flow_statement, statement
  PARGCHK();
  PARGNCHK(5);
  LOG << "let_statement_end";
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2],
      "Internal parser error.");
  BasicObjectPtr ast = args[3]->Get(sym::ast());
  RefExpression* ref = ast->GetData<RefExpression>();
  if(ref) {
    // Left side is variable.
    PARSER_CHECK(args[4]->HasSlot(sym::ast()), args[2],
        "Internal parser error.");
    return CreateTrue(args[2],
                      L(ref->Env(),
                        ref->Name(),
                        args[4]->Get(sym::ast())));
  } else {
    // Left side is array access?
    BasicObjectPtr target;
    BasicObjectPtr index;
    PARSER_CHECK(TryGetGetProc(ast, &target, &index), args[2],
        "Let need RefExpression in left side.");
    PARSER_CHECK(args[4]->HasSlot(sym::ast()), args[2],
        "Internal parser error.");
    return CreateTrue(args[2],
        CreateAst(args[1], args[2],
          target, sym::set(),
          index, args[4]->Get(sym::ast())));
  }
}

}  // namespace sru_parser
