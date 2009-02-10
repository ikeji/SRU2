// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#include "ast.h"

#include <string>
#include <vector>
#include "object_vector.h"

using namespace sru;
using namespace std;

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
BasicObject* LetExpression::Env(){
  return pimpl->env;
}
const string& LetExpression::Name(){
  return pimpl->name;
}
BasicObject* LetExpression::RightValue(){
  return pimpl->rightvalue;
}

void LetExpression::Mark(){
  pimpl->env->Mark();
  pimpl->rightvalue->Mark();
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
BasicObject* RefExpression::Env(){
  return pimpl->env;
}
const string& RefExpression::Name(){
  return pimpl->name;
}
void RefExpression::Mark(){
  pimpl->env->Mark();
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
BasicObject* CallExpression::Proc(){
  return pimpl->proc;
}
const object_vector& CallExpression::Arg(){
  return pimpl->arg;
}
void CallExpression::Mark(){
  pimpl->proc->Mark();
  for(object_vector::iterator it = pimpl->arg.begin();
      it != pimpl->arg.end();
      it++){
    (*it)->Mark();
  }
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
const vector<string>& ProcExpression::Varg(){
  return pimpl->varg;
}
const string& ProcExpression::RetVal(){
  return pimpl->retval;
}
const object_vector& ProcExpression::Expressions(){
  return pimpl->expressions;
}
void ProcExpression::Mark(){
  for(vector<BasicObject*>::iterator it = pimpl->expressions.begin();
      it != pimpl->expressions.end();
      it++){
    (*it)->Mark();
  }
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

