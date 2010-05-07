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

DEFINE_SRU_PROC(instance_method_begin){ // this, src, pos, 
  assert(args.size() >= 3);
  LOG << "instance_method_begin";
  // TODO: remove marker
  BasicObjectPtr r = R(sym::self());
  r->Set(sym::error(), Library::Instance()->Nil());
  return CreateTrue(args[2], E(args[1], args[2], r));
}

DEFINE_SRU_PROC(instance_method_self){ // this, src, pos, instance_method_begin, method_call
  assert(args.size() >= 5);
  LOG << "instance_method_self";
  BasicObjectPtr method_call = args[4]->Get(sym::ast());
  args[3]->Set(sym::ast(), method_call);
  return CreateTrue(args[2], method_call);
}

DEFINE_SRU_PROC(instance_method_method_begin){ // this, src, pos, ident
  assert(args.size() >= 3);
  LOG << "instance_method_method_begin";
  BasicObjectPtr r = args[3]->Get(sym::ast());
  RefExpression* ref = r->GetData<RefExpression>();
  CHECK(ref) << "instance_method_method_begin needs ref expression : " << args[3]->Inspect();
  return CreateTrue(args[2], E(args[1], args[2],
                               C(
                                 R(R(sym::doldol()),ref->Name()),
                                 R(sym::doldol())
                               )
                             ));
}

DEFINE_SRU_PROC(instance_method_method_arg){ // this, src, pos, instance_method_method_begin, statement
  assert(args.size() >= 5);
  LOG << "instance_method_method_arg";
  BasicObjectPtr c = args[3]->Get(sym::ast());
  CallExpression* call = c->GetData<CallExpression>();
  CHECK(call) << "instance_method_method_arg needs call expression : " << args[3]->Inspect();
  call->Arg()->push_back(args[4]->Get(sym::ast()).get());
  return CreateTrue(args[2], c);
}

DEFINE_SRU_PROC(instance_method_method_end){ // this, src, pos, instance_method_method_begin, instance_method_begin
  assert(args.size() >= 5);
  LOG << "instance_method_method_end";
  BasicObjectPtr self = args[4]->Get(sym::ast());
  BasicObjectPtr call =
    E(args[1], args[2], C(
      P(sym::doldol(),
        args[3]->Get(sym::ast())
      ),
      self
    ));
  args[4]->Set(sym::ast(), call);
  return CreateTrue(args[2], call);
}

DEFINE_SRU_PROC(instance_method_ref){ // this, src, pos, instance_method_begin, ident
  assert(args.size() >= 5);
  LOG << "instance_method_ref";
  BasicObjectPtr self = args[3]->Get(sym::ast());
  RefExpression* id = args[4]->Get(sym::ast())->GetData<RefExpression>();
  CHECK(id) << "instance_method_ref needs RefExpression.";
  BasicObjectPtr ident = id->Name();
  BasicObjectPtr ref = R(self, ident);
  args[3]->Set(sym::ast(), ref);
  return CreateTrue(args[2], ref);
  
}

DEFINE_SRU_PROC(instance_method_call_begin){ // this, src, pos, instance_method_begin
  assert(args.size() >= 3);
  LOG << "instance_method_call_begin";
  BasicObjectPtr self = args[3]->Get(sym::ast());
  return CreateTrue(args[2], E(args[1], args[2], C(self)));
}

DEFINE_SRU_PROC(instance_method_call_arg){ // this, src, pos, instance_method_call_begin, statement
  assert(args.size() >= 5);
  LOG << "instance_method_call_arg";
  BasicObjectPtr c = args[3]->Get(sym::ast());
  CallExpression* call = c->GetData<CallExpression>();
  CHECK(call) << "instance_method_call_arg needs call expression : " << args[3]->Inspect();
  call->Arg()->push_back(args[4]->Get(sym::ast()).get());
  return CreateTrue(args[2], c);

}

DEFINE_SRU_PROC(instance_method_call_end){ // this, src, pos, instance_method_begin, instance_method_call_begin
  assert(args.size() >= 5);
  LOG << "instance_method_call_end";
  args[3]->Set(sym::ast(), args[4]->Get(sym::ast()));
  return CreateTrue(args[2], args[3]->Get(sym::ast()));
}

DEFINE_SRU_PROC(instance_method_call_index){ // this, src, pos, instance_method_begin, statement
  assert(args.size() >= 5);
  LOG << "instance_method_call_index";
  BasicObjectPtr ast = E(args[1], args[2],
    C(
      P(sym::doldol(),
        E(args[1], args[2],
          C(R(R(sym::doldol()), sym::get()),
            R(sym::doldol()),
            args[4]->Get(sym::ast())
          ) 
        )
      ),
      args[3]->Get(sym::ast())
    ));
  args[3]->Set(sym::ast(), ast);
  return CreateTrue(args[2], ast);
}

DEFINE_SRU_PROC(instance_method_end){ // this, src, pos, instance_method_begin, method_call
  assert(args.size() >= 4);
  LOG << "instance_method_end";
  BasicObjectPtr self = args[3]->Get(sym::ast());
  if (self->HasSlot(sym::error())){
    return args[4];
  }
  return CreateTrue(args[2], self);
}

}  // namespace sru_parser
