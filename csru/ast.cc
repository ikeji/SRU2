// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
//
// Please see ast.h.

#include "ast.h"

#include <string>
#include <vector>
#include "object_container.h"
#include "library.h"
#include "string.h"

using namespace sru;
using namespace std;

string InspectExpression(BasicObjectPtr obj){
  Expression* exp = obj->GetData<Expression>();
  CHECK(exp) << "Couldn't Inspect " << obj->Inspect() <<
                " as a Expression";
  return exp->InspectAST();
}

/* -------------------- LetExpression -------------------- */

struct LetExpression::Impl {
  Impl(BasicObject* env, BasicObject* name, BasicObject* rightvalue) :
      env(env), name(name), rightvalue(rightvalue){}
  BasicObject* env;
  BasicObject* name;
  BasicObject* rightvalue;
 private:
  Impl(const Impl& obj);
  Impl* operator=(const Impl& obj);
};

LetExpression::LetExpression(const BasicObjectPtr& env,
                             const BasicObjectPtr& name,
                             const BasicObjectPtr& rightvalue) :
      pimpl(new Impl(env.get(),name.get(),rightvalue.get())) {
}
LetExpression::~LetExpression(){
  delete pimpl;
}
BasicObjectPtr LetExpression::Env(){
  return pimpl->env;
}
BasicObjectPtr LetExpression::SetEnv(BasicObjectPtr env){
  pimpl->env = env.get();
  return env;
}
BasicObjectPtr LetExpression::Name(){
  return pimpl->name;
}
BasicObjectPtr LetExpression::RightValue(){
  return pimpl->rightvalue;
}

void LetExpression::Mark(){
  if(pimpl->env) pimpl->env->Mark();
  if(pimpl->name) pimpl->name->Mark();
  if(pimpl->rightvalue) pimpl->rightvalue->Mark();
}

string LetExpression::InspectAST(){
  string rv = InspectExpression(pimpl->rightvalue);
  if(pimpl->env && Library::Instance()->Nil().get() != pimpl->env){
    return string("((") + InspectExpression(pimpl->env) + ")." +
           SRUString::GetValue(pimpl->name).to_str() +
           " = " + rv + ")";
  }else{
    return string("(") + SRUString::GetValue(pimpl->name).to_str() + " = " + rv + ")";
  }
}

/* -------------------- RefExpression -------------------- */

struct RefExpression::Impl {
  Impl(BasicObject* env, BasicObject* name) :
      env(env), name(name){}
  BasicObject* env;
  BasicObject* name;
 private:
  Impl(const Impl& obj);
  Impl* operator=(const Impl& obj);
};

RefExpression::RefExpression(const BasicObjectPtr& env,
                             const BasicObjectPtr& name):
    pimpl(new Impl(env.get(), name.get())){
}
RefExpression::~RefExpression(){
  delete pimpl;
}
BasicObjectPtr RefExpression::Env(){
  return pimpl->env;
}
BasicObjectPtr RefExpression::SetEnv(BasicObjectPtr env){
  pimpl->env = env.get();
  return env;
}
BasicObjectPtr RefExpression::Name(){
  return pimpl->name;
}
void RefExpression::Mark(){
  if(pimpl->env) pimpl->env->Mark();
  if(pimpl->name) pimpl->name->Mark();
}

string RefExpression::InspectAST(){
  if(pimpl->env && Library::Instance()->Nil().get() != pimpl->env){
    return string("(") + InspectExpression(pimpl->env) + ")." + 
           SRUString::GetValue(pimpl->name).to_str();
  }else{
    return SRUString::GetValue(pimpl->name).to_str();
  }
}

/* -------------------- CallExpression -------------------- */

struct CallExpression::Impl {
  Impl(BasicObject* proc, object_vector arg):
      proc(proc), arg(arg){}
  BasicObject* proc;
  object_vector arg;
 private:
  Impl(const Impl& obj);
  Impl* operator=(const Impl& obj);
};

CallExpression::CallExpression(const BasicObjectPtr& proc,
                               const ptr_vector& arg):
      pimpl(new Impl(proc.get(), Conv(arg))){
}
CallExpression::~CallExpression(){
  delete pimpl;
}
BasicObjectPtr CallExpression::Proc(){
  return pimpl->proc;
}
object_vector* CallExpression::Arg(){
  return &(pimpl->arg);
}
void CallExpression::Mark(){
  if(pimpl->proc) pimpl->proc->Mark();
  for(object_vector::iterator it = pimpl->arg.begin();
      it != pimpl->arg.end();
      it++){
    CHECK(*it) << "Why arg has NULL?";
    (*it)->Mark();
  }
}

string CallExpression::InspectAST(){
  string result = "";
  object_vector::iterator it = pimpl->arg.begin();
  while(true){
    if(it == pimpl->arg.end()) break;
    result += InspectExpression(*it);
    it++;
    if(it == pimpl->arg.end()) break;
    result += ", ";
  }
  return InspectExpression(pimpl->proc) + "(" + result + ")";
}

/* -------------------- ProcExpression -------------------- */

struct ProcExpression::Impl {
  Impl(vector<symbol> varg, symbol retval, object_vector expressions):
      varg(varg),retval(retval),expressions(expressions){}
  vector<symbol> varg;
  symbol retval;
  object_vector expressions;
 private:
  Impl(const Impl& obj);
  Impl* operator=(const Impl& obj);
};

ProcExpression::ProcExpression(const vector<symbol>& varg,
                               const symbol& retval,
                               const ptr_vector& expressions):
      pimpl(new Impl(varg,retval,Conv(expressions))){
}
ProcExpression::~ProcExpression(){
  delete pimpl;
}
vector<symbol>* ProcExpression::Varg(){
  return &(pimpl->varg);
}
const symbol& ProcExpression::RetVal(){
  return pimpl->retval;
}
object_vector* ProcExpression::Expressions(){
  return &(pimpl->expressions);
}
void ProcExpression::Mark(){
  for(object_vector::iterator it = pimpl->expressions.begin();
      it != pimpl->expressions.end();
      it++){
    CHECK(*it) << "Why expression has NULL?";
    (*it)->Mark();
  }
}

string ProcExpression::InspectAST(){
  string argv = "";
  vector<symbol>::iterator i = pimpl->varg.begin();
  while(true){
    if(i == pimpl->varg.end()) break;
    argv+= i->to_str();
    i++;
    if(i == pimpl->varg.end()) break;
    argv+= ",";
  }
  if(!pimpl->retval.to_str().empty())
    argv+= ":" + pimpl->retval.to_str();
  if(!argv.empty())
    argv = "|" + argv + "|";
  string main = "";
  for(object_vector::iterator it = pimpl->expressions.begin();
      it != pimpl->expressions.end();
      it++){
    main+= InspectExpression(*it) + ";";
  }
  return string("{") + argv + main + "}";
}

/* -------------------- StringExpression -------------------- */

struct StringExpression::Impl {
  Impl(symbol str) : str(str) {}
  symbol str;
 private:
  Impl(const Impl& obj);
  Impl* operator=(const Impl& obj);
};

StringExpression::StringExpression(const symbol& str):
    pimpl(new Impl(str)){
}
StringExpression::~StringExpression(){
  delete pimpl;
}
const symbol& StringExpression::String(){
  return pimpl->str;
}

string StringExpression::InspectAST(){
  return string("\"") + pimpl->str.to_str() + "\"";
}
