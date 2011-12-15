// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 
#include "ast_class.h"

#include "basic_object.h"
#include "constants.h"
#include "class.h"
#include "library.h"
#include "ast.h"
#include "symbol.h"
#include "native_proc.h"
#include "sru_string.h"

using namespace sru;
using namespace std;

struct ASTClass::Impl {
  Impl():
    AstClass(),
    LetClass(),
    RefClass(),
    CallClass(),
    ProcClass(),
    StringClass() {}
  BasicObjectPtr AstClass;
  BasicObjectPtr LetClass;
  BasicObjectPtr RefClass;
  BasicObjectPtr CallClass;
  BasicObjectPtr ProcClass;
  BasicObjectPtr StringClass;
};

DEFINE_SRU_PROC(StringExpressionNew){
  ARGLEN(2);
  symbol s = SRUString::GetValue(args[1]);
  BasicObjectPtr se = StringExpression::New(s);
  return se;
}

ASTClass::ASTClass(const Library& lib) : pimpl(new Impl){
  pimpl->AstClass = Class::New(lib.Object());
  pimpl->LetClass = Class::New(pimpl->AstClass);
  pimpl->RefClass = Class::New(pimpl->AstClass);
  pimpl->CallClass = Class::New(pimpl->AstClass);
  pimpl->ProcClass = Class::New(pimpl->AstClass);
  pimpl->StringClass = Class::New(pimpl->AstClass);
  pimpl->StringClass->Set(sym::mew(), CREATE_SRU_PROC(StringExpressionNew));
}

ASTClass::~ASTClass(){
  delete pimpl;
}

BasicObjectPtr ASTClass::GetASTNamespace(){
  BasicObjectPtr obj = BasicObject::New();
  obj->Set(sym::AST(), pimpl->AstClass);
  obj->Set(sym::LetExpression(), pimpl->LetClass);
  obj->Set(sym::RefExpression(), pimpl->RefClass);
  obj->Set(sym::CallExpression(), pimpl->CallClass);
  obj->Set(sym::ProcExpression(), pimpl->ProcClass);
  obj->Set(sym::StringExpression(), pimpl->StringClass);
  return obj;
}

BasicObjectPtr ASTClass::GetLetExpressionClass(){
  return pimpl->LetClass;
}

BasicObjectPtr ASTClass::GetRefExpressionClass(){
  return pimpl->RefClass;
}

BasicObjectPtr ASTClass::GetCallExpressionClass(){
  return pimpl->CallClass;
}

BasicObjectPtr ASTClass::GetProcExpressionClass(){
  return pimpl->ProcClass;
}

BasicObjectPtr ASTClass::GetStringExpressionClass(){
  return pimpl->StringClass;
}

