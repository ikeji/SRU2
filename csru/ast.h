// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 
// Abstract Syntax Tree
//
// In SRU, AST is an object.
// User can make instance of AST.
// All Expressions are an Value and need impliment Mark procedure.
// Expression have thease 5 types.
// LetExpression and RefExpression controll binding.
// CallExpression make call function.
// ProcExpression and StringExpression are seeds for Constants.
//

#ifndef AST_H_
#define AST_H_

#include <vector>
#include <string>
#include "basic_object.h"
#include "object_container.h"

namespace sru {

class Visitor;

class Expression : public Value{
 public:
  Expression(){}
  // TODO: use template?
  virtual void Visit(Visitor* visitor,BasicObjectPtr obj) = 0;
  virtual std::string Inspect(){
    return std::string("Expression(") + InspectAST() + ")";
  }
  virtual std::string InspectAST() = 0;
  static const char* name(){ return "Expression"; }
  void Dispose();
 private:
  Expression(const Expression& obj);
  Expression& operator=(const Expression& obj);
};

class LetExpression;
class RefExpression;
class CallExpression;
class ProcExpression;
class StringExpression;

class Visitor{
 public:
  virtual ~Visitor(){}
  virtual void Accept(LetExpression* exp,const BasicObjectPtr& obj) = 0;
  virtual void Accept(RefExpression* exp,const BasicObjectPtr& obj) = 0;
  virtual void Accept(CallExpression* exp,const BasicObjectPtr& obj) = 0;
  virtual void Accept(ProcExpression* exp,const BasicObjectPtr& obj) = 0;
  virtual void Accept(StringExpression* exp,const BasicObjectPtr& obj) = 0;
};

class LetExpression : public Expression{
 public:
  static BasicObjectPtr New(
      const BasicObjectPtr& env,
      const BasicObjectPtr& name,
      const BasicObjectPtr& rightvalue);

  LetExpression(const BasicObjectPtr& env,
                const BasicObjectPtr& name,
                const BasicObjectPtr& rightvalue);
  ~LetExpression();

  void Visit(Visitor* visitor,BasicObjectPtr obj){
    visitor->Accept(this,obj);
  }
  void Mark();

  BasicObjectPtr Env();
  BasicObjectPtr SetEnv(BasicObjectPtr env);
  BasicObjectPtr Name();
  BasicObjectPtr RightValue();
  
  std::string InspectAST();
 private:
  struct Impl;
  Impl* pimpl;

  LetExpression(const LetExpression& obj);
  LetExpression& operator=(const LetExpression& obj);
};

class RefExpression : public Expression{
 public:
  static BasicObjectPtr New(
      const BasicObjectPtr& env,
      const BasicObjectPtr& name);

  RefExpression(
      const BasicObjectPtr& env,
      const BasicObjectPtr& name);
  ~RefExpression();

  void Visit(Visitor* visitor,BasicObjectPtr obj){
    visitor->Accept(this,obj);
  }
  void Mark();

  BasicObjectPtr Env();
  BasicObjectPtr SetEnv(BasicObjectPtr env);
  BasicObjectPtr Name();
  
  std::string InspectAST();
 private:
  struct Impl;
  Impl* pimpl;

  RefExpression(const RefExpression& obj);
  RefExpression& operator=(const RefExpression& obj);
};

class CallExpression : public Expression{
 public:
  static BasicObjectPtr New(
      const BasicObjectPtr& proc,
      const ptr_vector& arg);

  CallExpression(
      const BasicObjectPtr& proc,
      const ptr_vector& arg);
  ~CallExpression();

  void Visit(Visitor* visitor,BasicObjectPtr obj){
    visitor->Accept(this,obj);
  }
  void Mark();

  BasicObjectPtr Proc();
  object_vector* Arg();

  std::string InspectAST();
 private:
  struct Impl;
  Impl* pimpl;

  CallExpression(const CallExpression& obj);
  CallExpression& operator=(const CallExpression& obj);
};

class ProcExpression : public Expression{
 public:
  static BasicObjectPtr New(
      const std::vector<sru::symbol>& varg,
      const sru::symbol& retval,
      const ptr_vector& expressions);

  ProcExpression(const std::vector<sru::symbol>& varg,
                 const sru::symbol& retval,
                 const ptr_vector& expressions);
  ~ProcExpression();

  void Visit(Visitor* visitor,BasicObjectPtr obj){
    visitor->Accept(this,obj);
  }
  void Mark();

  std::vector<sru::symbol>* Varg();
  void SetVarg(const std::vector<sru::symbol>& varg);
  const sru::symbol& RetVal();
  void SetRetVal(const sru::symbol& retval);
  object_vector* Expressions();
  void SetExpressions(const object_vector& expressions);
  
  std::string InspectAST();
 private:
  struct Impl;
  Impl* pimpl;

  ProcExpression(const ProcExpression& obj);
  ProcExpression& operator=(const ProcExpression& obj);
};

class StringExpression : public Expression{
 public:
  static BasicObjectPtr New(const sru::symbol& str);

  StringExpression(const sru::symbol& str);
  ~StringExpression();

  void Visit(Visitor* visitor,BasicObjectPtr obj){
    visitor->Accept(this,obj);
  }
 
  const sru::symbol& String();
  
  std::string InspectAST();
 private:
  struct Impl;
  Impl* pimpl;

  StringExpression(const StringExpression& obj);
  StringExpression& operator=(const StringExpression& obj);
};

#ifdef SRU_MAIN
inline BasicObjectPtr LetExpression::New(
    const BasicObjectPtr& env,
    const BasicObjectPtr& name,
    const BasicObjectPtr& rightvalue){
  const BasicObjectPtr r = BasicObject::New(
      new LetExpression(env,name,rightvalue));
  return r;
}

inline BasicObjectPtr RefExpression::New(
    const BasicObjectPtr& env,
    const BasicObjectPtr& name){
  const BasicObjectPtr r = BasicObject::New(
      new RefExpression(env,name));
  return r;
}
  
inline BasicObjectPtr CallExpression::New(
    const BasicObjectPtr& proc,
    const ptr_vector& arg){
  const BasicObjectPtr r = BasicObject::New(
      new CallExpression(proc,arg));
  return r;
}
  
inline BasicObjectPtr ProcExpression::New(
    const std::vector<sru::symbol>& varg,
    const sru::symbol& retval,
    const ptr_vector& expressions){
  const BasicObjectPtr r = BasicObject::New(
      new ProcExpression(varg,retval,expressions));
  return r;
}
  
inline BasicObjectPtr StringExpression::New(
    const sru::symbol& str){
  const BasicObjectPtr r = BasicObject::New(
      new StringExpression(str));
  return r;
}

#endif


} // namespace sru

#endif  // AST_H_
