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

DEFINE_SRU_PROC(instance_method_begin){ // this, src, pos, 
  PARSER_CHECK(args.size() >= 3, args[2], "Internal parser error.");
  LOG << "instance_method_begin";
  // TODO: remove marker
  BasicObjectPtr r = R(sym::self());
  r->Set(sym::error(), Library::Instance()->Nil());
  return CreateTrue(args[2], E(args[1], args[2], r));
}

DEFINE_SRU_PROC(instance_method_self){ // this, src, pos, instance_method_begin, method_call
  PARSER_CHECK(args.size() >= 5, args[2], "Internal parser error.");
  LOG << "instance_method_self";
  PARSER_CHECK(args[4]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  BasicObjectPtr method_call = args[4]->Get(sym::ast());
  args[3]->Set(sym::ast(), method_call);
  return CreateTrue(args[2], method_call);
}

DEFINE_SRU_PROC(instance_method_method_begin){ // this, src, pos, ident
  PARSER_CHECK(args.size() >= 3, args[2], "Internal parser error.");
  LOG << "instance_method_method_begin";
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  BasicObjectPtr r = args[3]->Get(sym::ast());
  RefExpression* ref = r->GetData<RefExpression>();
  PARSER_CHECK(ref, args[2],
      "instance_method_method_begin needs ref expression");
  return CreateTrue(args[2], E(args[1], args[2],
                               C(
                                 R(R(sym::doldol()),ref->Name()),
                                 R(sym::doldol())
                               )
                             ));
}

DEFINE_SRU_PROC(instance_method_method_arg){ // this, src, pos, instance_method_method_begin, statement
  PARSER_CHECK(args.size() >= 5, args[2], "Internal parser error.");
  LOG << "instance_method_method_arg";
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  BasicObjectPtr c = args[3]->Get(sym::ast());
  CallExpression* call = c->GetData<CallExpression>();
  PARSER_CHECK(call, args[2],
      "instance_method_method_arg needs call expression");
  PARSER_CHECK(args[4]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  call->Arg()->push_back(args[4]->Get(sym::ast()).get());
  return CreateTrue(args[2], c);
}

DEFINE_SRU_PROC(instance_method_method_end){ // this, src, pos, instance_method_method_begin, instance_method_begin
  PARSER_CHECK(args.size() >= 5, args[2], "Internal parser error.");
  LOG << "instance_method_method_end";
  PARSER_CHECK(args[4]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  BasicObjectPtr self = args[4]->Get(sym::ast());
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
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
  PARSER_CHECK(args.size() >= 5, args[2], "Internal parser error.");
  LOG << "instance_method_ref";
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  BasicObjectPtr self = args[3]->Get(sym::ast());
  PARSER_CHECK(args[4]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  RefExpression* id = args[4]->Get(sym::ast())->GetData<RefExpression>();
  PARSER_CHECK(id, args[2],
      "instance_method_ref needs RefExpression.");
  BasicObjectPtr ident = id->Name();
  BasicObjectPtr ref = R(self, ident);
  args[3]->Set(sym::ast(), ref);
  return CreateTrue(args[2], ref);
  
}

DEFINE_SRU_PROC(instance_method_call_begin){ // this, src, pos, instance_method_begin
  PARSER_CHECK(args.size() >= 3, args[2], "Internal parser error.");
  LOG << "instance_method_call_begin";
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  BasicObjectPtr self = args[3]->Get(sym::ast());
  return CreateTrue(args[2], E(args[1], args[2], C(self)));
}

DEFINE_SRU_PROC(instance_method_call_arg){ // this, src, pos, instance_method_call_begin, statement
  PARSER_CHECK(args.size() >= 5, args[2], "Internal parser error.");
  LOG << "instance_method_call_arg";
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  BasicObjectPtr c = args[3]->Get(sym::ast());
  CallExpression* call = c->GetData<CallExpression>();
  PARSER_CHECK(call, args[2],
      "instance_method_call_arg needs call expression");
  PARSER_CHECK(args[4]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  call->Arg()->push_back(args[4]->Get(sym::ast()).get());
  return CreateTrue(args[2], c);

}

DEFINE_SRU_PROC(instance_method_call_end){ // this, src, pos, instance_method_begin, instance_method_call_begin
  PARSER_CHECK(args.size() >= 5, args[2], "Internal parser error.");
  LOG << "instance_method_call_end";
  PARSER_CHECK(args[4]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  args[3]->Set(sym::ast(), args[4]->Get(sym::ast()));
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  return CreateTrue(args[2], args[3]->Get(sym::ast()));
}

DEFINE_SRU_PROC(instance_method_call_index){ // this, src, pos, instance_method_begin, statement
  PARSER_CHECK(args.size() >= 5, args[2], "Internal parser error.");
  LOG << "instance_method_call_index";
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  PARSER_CHECK(args[4]->HasSlot(sym::ast()), args[2], "Internal parser error.");
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
  PARSER_CHECK(args.size() >= 4, args[2], "Internal parser error.");
  LOG << "instance_method_end";
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  BasicObjectPtr self = args[3]->Get(sym::ast());
  if (self->HasSlot(sym::error())){
    return args[4];
  }
  return CreateTrue(args[2], self);
}

}  // namespace sru_parser
