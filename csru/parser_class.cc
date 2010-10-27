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
#include "utils.h"

// TODO: remove this dependency
#include "testing_ast.h"

using namespace sru;
using namespace std;
using namespace sru_test;

namespace sru_parser {

DEFINE_SRU_PROC(class_statement_begin){ // this, src, pos, ident, statement
  PARGCHK();
  PARGNCHK(5);
  LOG << "class_statement_begin";
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  RefExpression* ident = args[3]->Get(sym::ast())->GetData<RefExpression>();
  PARSER_CHECK(ident,args[2],
      "Class statement need RefExpression as name.");
  BasicObjectPtr superClass;
  PARSER_CHECK(
      args[4].get() == Library::Instance()->Nil().get() ||
      args[4]->HasSlot(sym::status()),
      args[2], "Internal parser error.");
  if(!IsNil(args[4]) &&
     args[4]->Get(sym::status()) == Library::Instance()->True()){
    LOG << "Use super class";
    PARSER_CHECK(args[4]->HasSlot(sym::ast()), args[2], "Internal parser error.");
    superClass = args[4]->Get(sym::ast());
  } else {
    LOG << "Use Object class";
    superClass = R(sym::Object());
  }
  const BasicObjectPtr ast = P(
      L(sym::doldol(),
        CreateAst(args[1],args[2],
                  superClass,
                  sym::subclass(),
                  S(SRUString::GetValue(ident->Name())))));
  // We already check args[3]->Get(ast).
  ast->Set(sym::__name(), args[3]->Get(sym::ast()));
  return CreateTrue(args[2], ast);
}

DEFINE_SRU_PROC(class_statement_method_begin){ // this, src, pos, ident
  PARGCHK();
  PARGNCHK(4);
  LOG << "class_statement_method_begin";
  const BasicObjectPtr ast = P();
  ast->GetData<ProcExpression>()->SetRetVal(sym::leturn());
  ast->GetData<ProcExpression>()->Varg()->push_back(sym::self());
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  // TODO: use string instead of ref expression.
  ast->Set(sym::__name(), args[3]->Get(sym::ast()));
  return CreateTrue(args[2], ast);
}

DEFINE_SRU_PROC(class_statement_method_varg){ // this, src, pos, class_statement_method_begin, ident
  PARGCHK();
  PARGNCHK(5);
  LOG << "class_statement_method_varg";
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  ProcExpression* p = args[3]->Get(sym::ast())->GetData<ProcExpression>();
  PARSER_CHECK(p, args[2],
      "Class statement method varg need ProcExpression.");
  PARSER_CHECK(args[4]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  RefExpression* ref = args[4]->Get(sym::ast())->GetData<RefExpression>();
  PARSER_CHECK(ref, args[2],
      "Class statement method varg need RefExpression.");
  p->Varg()->push_back(SRUString::GetValue(ref->Name()));
  return args[4];
}

DEFINE_SRU_PROC(class_statement_method_end){ // this, src, pos, class_statement_begin, class_statement_method_begin, statements
  PARGCHK();
  PARGNCHK(6);
  LOG << "class_statement_method_end";
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  ProcExpression* p = args[3]->Get(sym::ast())->GetData<ProcExpression>();
  PARSER_CHECK(p, args[2],
      "Class statement method end need ProcExpression.");
  PARSER_CHECK(args[4]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  PARSER_CHECK(args[4]->Get(sym::ast())->HasSlot(sym::__name()),
      args[2], "Internal parser error.");
  RefExpression* ref = args[4]->Get(sym::ast())->
      Get(sym::__name())->
      GetData<RefExpression>();
  PARSER_CHECK(ref, args[2],
      "Class statement method end need RefExpression.");
  ProcExpression* method = args[4]->Get(sym::ast())->GetData<ProcExpression>();
  PARSER_CHECK(method, args[2],
      "Class statement method end need ProcExpression as method.");
  PARSER_CHECK(args[5]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  ProcExpression* statement = args[5]->Get(sym::ast())->GetData<ProcExpression>();
  PARSER_CHECK(statement, args[2],
      "Class statement method end need ProcExpression as statement.");

  for (object_vector::iterator it = statement->Expressions()->begin();
       it != statement->Expressions()->end();
       it++) {
    method->Expressions()->push_back(*it);
  }

  p->Expressions()->push_back(
      L(R(R(sym::doldol()), sym::instanceMethods()),
        ref->Name(),
        args[4]->Get(sym::ast())).get());
  return CreateTrue(args[2], args[3]->Get(sym::ast()));
}

DEFINE_SRU_PROC(class_statement_end){ // this, src, pos, class_statement_begin
  PARGCHK();
  PARGNCHK(4);
  LOG << "class_statement_end";
  PARSER_CHECK(args[3]->HasSlot(sym::ast()), args[2], "Internal parser error.");
  ProcExpression* p = args[3]->Get(sym::ast())->GetData<ProcExpression>();
  PARSER_CHECK(p, args[2],
      "Class statement method end need ProcExpression.");
  PARSER_CHECK(args[3]->Get(sym::ast())->HasSlot(sym::__name()),
      args[2], "Internal parser error.");
  RefExpression* ref = args[3]->Get(sym::ast())->
      Get(sym::__name())->GetData<RefExpression>();
  PARSER_CHECK(ref, args[2],
      "Class statement end need RefExpression.");

  p->Expressions()->push_back(R(sym::doldol()).get());

  return CreateTrue(args[2], L(ref->Name(),
                               C(args[3]->Get(sym::ast()))));
}

}  // namespace sru_parser

