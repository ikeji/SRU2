// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
//
// Please see ast.h.

#include "ast.h"

#include <string>
#include <vector>
#include "object_vector.h"
#include "library.h"

using namespace sru;
using namespace std;

string InspectExpression(BasicObjectPtr obj){
  return obj->GetData<Expression>()->InspectAST();
}

/* -------------------- LetExpression -------------------- */

struct LetExpression::Impl {
  Impl(BasicObject* env, const string& name, BasicObject* rightvalue) :
      env(env), name(name), rightvalue(rightvalue){}
  BasicObject* env;
  string name;
  BasicObject* rightvalue;
 private:
  Impl(const Impl& obj);
  Impl* operator=(const Impl& obj);
};

LetExpression::LetExpression(const BasicObjectPtr& env,
                             const string& name,
                             const BasicObjectPtr& rightvalue) :
      pimpl(new Impl(env.get(),name,rightvalue.get())) {
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
const string& LetExpression::Name(){
  return pimpl->name;
}
BasicObjectPtr LetExpression::RightValue(){
  return pimpl->rightvalue;
}

void LetExpression::Mark(){
  if(pimpl->env) pimpl->env->Mark();
  if(pimpl->rightvalue) pimpl->rightvalue->Mark();
}

string LetExpression::InspectAST(){
  string rv = InspectExpression(pimpl->rightvalue);
  if(pimpl->env && Library::Instance()->Nil().get() != pimpl->env){
    return string("((") + InspectExpression(pimpl->env) + ")." + pimpl->name +
           " = " + rv + ")";
  }else{
    return string("(") + pimpl->name + " = " + rv + ")";
  }
}

/* -------------------- RefExpression -------------------- */

struct RefExpression::Impl {
  Impl(BasicObject* env, string name) :
      env(env), name(name){}
  BasicObject* env;
  string name;
 private:
  Impl(const Impl& obj);
  Impl* operator=(const Impl& obj);
};

RefExpression::RefExpression(const BasicObjectPtr& env,
                             const string& name):
    pimpl(new Impl(env.get(), name)){
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
const string& RefExpression::Name(){
  return pimpl->name;
}
void RefExpression::Mark(){
  if(pimpl->env) pimpl->env->Mark();
}

string RefExpression::InspectAST(){
  if(pimpl->env && Library::Instance()->Nil().get() != pimpl->env){
    return string("(") + InspectExpression(pimpl->env) + ")." + pimpl->name;
  }else{
    return pimpl->name;
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
  Impl(vector<string> varg, string retval, object_vector expressions):
      varg(varg),retval(retval),expressions(expressions){}
  vector<string> varg;
  string retval;
  object_vector expressions;
 private:
  Impl(const Impl& obj);
  Impl* operator=(const Impl& obj);
};

ProcExpression::ProcExpression(const vector<string>& varg,
                               const string& retval,
                               const ptr_vector& expressions):
      pimpl(new Impl(varg,retval,Conv(expressions))){
}
ProcExpression::~ProcExpression(){
  delete pimpl;
}
vector<string>* ProcExpression::Varg(){
  return &(pimpl->varg);
}
const string& ProcExpression::RetVal(){
  return pimpl->retval;
}
object_vector* ProcExpression::Expressions(){
  return &(pimpl->expressions);
}
void ProcExpression::Mark(){
  for(object_vector::iterator it = pimpl->expressions.begin();
      it != pimpl->expressions.end();
      it++){
    (*it)->Mark();
  }
}

string ProcExpression::InspectAST(){
  string argv = "";
  vector<string>::iterator i = pimpl->varg.begin();
  while(true){
    if(i == pimpl->varg.end()) break;
    argv+= *i;
    i++;
    if(i == pimpl->varg.end()) break;
    argv+= ",";
  }
  if(!pimpl->retval.empty())
    argv+= ":" + pimpl->retval;
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
  Impl(string str) : str(str) {}
  string str;
 private:
  Impl(const Impl& obj);
  Impl* operator=(const Impl& obj);
};

StringExpression::StringExpression(const string& str):
    pimpl(new Impl(str)){
}
StringExpression::~StringExpression(){
  delete pimpl;
}
const string& StringExpression::String(){
  return pimpl->str;
}

string StringExpression::InspectAST(){
  return string("\"") + pimpl->str + "\"";
}
