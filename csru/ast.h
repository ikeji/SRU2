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
#include "object_vector.h"

namespace sru {

class Visitor;

class Expression : public Value{
 public:
  Expression(){}
  // TODO: use template?
  virtual void Visit(Visitor* visitor,BasicObjectPtr obj) = 0;
  virtual std::string Inspect(){
    return std::string("<Expression:") + InspectAST() + ">";
  }
  virtual std::string InspectAST() = 0;
 private:
  Expression(const Expression& obj);
  Expression* &operator=(const Expression& obj);
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
  static BasicObjectPtr New(const BasicObjectPtr& env,
                            const std::string& name,
                            const BasicObjectPtr& rightvalue){
    BasicObjectPtr r = BasicObject::New(
        new LetExpression(env,name,rightvalue));
    return r;
  }

  LetExpression(const BasicObjectPtr& env,
                const std::string& name,
                const BasicObjectPtr& rightvalue);
  ~LetExpression();

  void Visit(Visitor* visitor,BasicObjectPtr obj){
    visitor->Accept(this,obj);
  }
  void Mark();

  BasicObject* Env();
  const std::string& Name();
  BasicObject* RightValue();
  
  std::string InspectAST();
 private:
  struct Impl;
  Impl* pimpl;

  LetExpression(const LetExpression& obj);
  LetExpression* &operator=(const LetExpression& obj);
};

class RefExpression : public Expression{
 public:
  static BasicObjectPtr New(const BasicObjectPtr& env,
                            const std::string& name){
    BasicObjectPtr r = BasicObject::New(
        new RefExpression(env,name));
    return r;
  }

  RefExpression(const BasicObjectPtr& env, const std::string& name);
  ~RefExpression();

  void Visit(Visitor* visitor,BasicObjectPtr obj){
    visitor->Accept(this,obj);
  }
  void Mark();

  BasicObject* Env();
  const std::string& Name();
  
  std::string InspectAST();
 private:
  struct Impl;
  Impl* pimpl;

  RefExpression(const RefExpression& obj);
  RefExpression* &operator=(const RefExpression& obj);
};

class CallExpression : public Expression{
 public:
  static BasicObjectPtr New(const BasicObjectPtr& proc,
             const ptr_vector& arg){
    BasicObjectPtr r = BasicObject::New(
        new CallExpression(proc,arg));
    return r;
  }

  CallExpression(const BasicObjectPtr& proc,
                 const ptr_vector& arg);
  ~CallExpression();

  void Visit(Visitor* visitor,BasicObjectPtr obj){
    visitor->Accept(this,obj);
  }
  void Mark();

  BasicObject* Proc();
  const object_vector& Arg();

  std::string InspectAST();
 private:
  struct Impl;
  Impl* pimpl;

  CallExpression(const CallExpression& obj);
  CallExpression* &operator=(const CallExpression& obj);
};

class ProcExpression : public Expression{
 public:
  static BasicObjectPtr New(const std::vector<std::string>& varg,
                            const std::string& retval,
                            const ptr_vector& expressions){
    BasicObjectPtr r = BasicObject::New(
        new ProcExpression(varg,retval,expressions));
    return r;
  }

  ProcExpression(const std::vector<std::string>& varg,
                 const std::string& retval,
                 const ptr_vector& expressions);
  ~ProcExpression();

  void Visit(Visitor* visitor,BasicObjectPtr obj){
    visitor->Accept(this,obj);
  }
  void Mark();

  const std::vector<std::string>& Varg();
  const std::string& RetVal();
  const object_vector& Expressions();
  
  std::string InspectAST();
 private:
  struct Impl;
  Impl* pimpl;

  ProcExpression(const ProcExpression& obj);
  ProcExpression* &operator=(const ProcExpression& obj);
};

class StringExpression : public Expression{
 public:
  static BasicObjectPtr New(const std::string& str){
    BasicObjectPtr r = BasicObject::New(
        new StringExpression(str));
    return r;
  }

  StringExpression(const std::string& str);
  ~StringExpression();

  void Visit(Visitor* visitor,BasicObjectPtr obj){
    visitor->Accept(this,obj);
  }
 
  const std::string& String();
  
  std::string InspectAST();
 private:
  struct Impl;
  Impl* pimpl;

  StringExpression(const StringExpression& obj);
  StringExpression* &operator=(const StringExpression& obj);
};

} // namespace sru

#endif  // AST_H_
