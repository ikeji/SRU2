// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#include "stack_frame.h"
#include "object_vector.h"
#include <vector>
#include "basic_object.h"
#include "string.h"
#include "ast.h"
#include "proc.h"

#include <iostream>
using namespace std;
using namespace sru;

struct StackFrame::Impl {
  Impl():
    expressions(),
    tree_it(0),
    operations(),
    it(0),
    local_stack(),
    binding(BasicObject::New().get()),
    up_frame(NULL)
    {}

  object_vector expressions;
  unsigned int tree_it;

  object_vector operations;
  unsigned int it;

  object_vector local_stack;
 
  BasicObject* binding; 
  BasicObject* up_frame;

  bool SetupTree(BasicObjectPtr ast);
 private:
  Impl(const Impl& obj);
  Impl* operator=(const Impl& obj);
};

BasicObjectPtr StackFrame::New(){
  return BasicObject::New(new StackFrame());
}

StackFrame::StackFrame():pimpl(new Impl()){
}

StackFrame::~StackFrame(){
  delete pimpl;
}

class TraceVisitor : public Visitor{
 public:
  TraceVisitor(object_vector* res):
        result(res),noerror(true){}
  void VisitTo(BasicObjectPtr obj){
    dynamic_cast<Expression*>(obj->Data())->Visit(this, obj);
  }
  void Accept(LetExpression* exp,const BasicObjectPtr& obj){
    if(exp->Env())
      VisitTo(exp->Env());
    VisitTo(exp->RightValue());
    result->push_back(obj.get());
  }
  void Accept(RefExpression* exp,const BasicObjectPtr& obj){
    if(exp->Env())
      VisitTo(exp->Env());
    result->push_back(obj.get());
  }
  void Accept(CallExpression* exp,const BasicObjectPtr& obj){
    VisitTo(exp->Proc());
    for(object_vector::const_iterator it = exp->Arg().begin();
        it != exp->Arg().end();
        it++)
      result->push_back(*it);
    result->push_back(obj.get());
  }
  void Accept(ProcExpression* exp,const BasicObjectPtr& obj){
    result->push_back(obj.get());
  }
  void Accept(StringExpression* exp,const BasicObjectPtr& obj){
    result->push_back(obj.get());
  }
  object_vector* result;
  bool noerror;
 private:
  TraceVisitor(const TraceVisitor& obj);
  TraceVisitor* operator=(const TraceVisitor& obj);
};

class EvalVisitor : public Visitor{
 public:
  EvalVisitor(object_vector* st,const BasicObjectPtr& binding):
      local_stack(st),binding(binding),noerror(true){}
  BasicObjectPtr Pop(){
    assert(local_stack->size() > 0);
    BasicObjectPtr r = local_stack->back();
    local_stack->pop_back();
    return r;
  }
  void Push(const BasicObjectPtr& obj){
    assert(obj.get());
    local_stack->push_back(obj.get());
  }
  void Accept(LetExpression* exp,const BasicObjectPtr& obj){
    const BasicObjectPtr& rightValue = Pop();
    BasicObjectPtr env;
    if(exp->Env()){
      env = Pop();
    }else{
      env = binding;
    }
    env->Set(exp->Name(),rightValue);
    Push(rightValue);
  }
  void Accept(RefExpression* exp,const BasicObjectPtr& obj){
    BasicObjectPtr env;
    if(exp->Env()){
      env = Pop();
    }else{
      env = binding;
    }
    Push(env->Get(exp->Name()));
  }
  void Accept(CallExpression* exp,const BasicObjectPtr& obj){
    // Get arg value from stack, but it on reverse order.
    ptr_vector reverse_args;
    for(object_vector::const_iterator it = exp->Arg().begin();
        it != exp->Arg().end();
        it++)
      reverse_args.push_back(Pop());

    // Reorder.
    ptr_vector args;
    for(ptr_vector::reverse_iterator it = reverse_args.rbegin();
        it != reverse_args.rend();
        it++)
      args.push_back(*it);

    BasicObjectPtr proc = Pop();
    Proc* p = dynamic_cast<Proc*>(proc->Data());
    assert(p || !"First error: Call target is must proc");
    p->Call(args);
    // Caller must push return value.
  }
  void Accept(ProcExpression* exp,const BasicObjectPtr& obj){
    Push(Proc::New(
             exp->Varg(),exp->RetVal(),
             Conv(exp->Expressions()),binding));
  }
  void Accept(StringExpression* exp,const BasicObjectPtr& obj){
    Push(SRUString::New(exp->String()));
  }
  object_vector* local_stack;
  BasicObjectPtr binding;
  bool noerror;
 private:
  EvalVisitor(const EvalVisitor& obj);
  EvalVisitor* operator=(const EvalVisitor& obj);
};

bool StackFrame::Impl::SetupTree(BasicObjectPtr ast){
#ifdef DEBUG
  cout << "SetupTree: " <<
       dynamic_cast<Expression*>(ast->Data())->Inspect() << endl;
#endif
  local_stack.clear();
  operations.clear();
  TraceVisitor tracer(&operations);
  tracer.VisitTo(ast);
  it = 0;
  return tracer.noerror;
}

void StackFrame::Setup(const ptr_vector& asts){
#ifdef DEBUG
  cout << "Setup: ";
  for(ptr_vector::const_iterator it = asts.begin();
      it != asts.end();
      it++){
    cout << dynamic_cast<Expression*>((*it)->Data())->Inspect();
  }
  cout << endl;
#endif
  pimpl->expressions = Conv(asts);
  pimpl->tree_it = 0;
  pimpl->operations.clear();
  pimpl->it = 0;
}

void StackFrame::SetUpStack(BasicObjectPtr obj){
  pimpl->up_frame = obj.get();
}

bool StackFrame::EndOfTrees(){
  return pimpl->up_frame == NULL &&
         pimpl->expressions.size() == pimpl->tree_it &&
         pimpl->operations.size() == pimpl->it;
}

bool StackFrame::EvalNode(){
  assert(!EndOfTrees());
  if(pimpl->operations.size() == pimpl->it){
    if(pimpl->expressions.size() == pimpl->tree_it){
      BasicObjectPtr rv = ReturnValue();
      StackFrame* st = dynamic_cast<StackFrame*>(pimpl->up_frame->Data());
      if(st == NULL)
        return false;
#ifdef DEBUG
      cout << "Step Out:" << pimpl->up_frame->Inspect() << endl;
#endif
      *this = *st;
      pimpl->local_stack.push_back(rv.get());
      return true;
    }
    bool ret = pimpl->SetupTree(pimpl->expressions[pimpl->tree_it]);
    (pimpl->tree_it)++;
    return ret;
  }
  EvalVisitor visit(&(pimpl->local_stack),pimpl->binding);
  // We need step forward before exec each code.
  BasicObjectPtr cur = pimpl->operations[pimpl->it];
  pimpl->it++;
  // Execute immidentry
  Expression* exp = dynamic_cast<Expression*>(cur->Data());
  assert(exp);
  exp->Visit(&visit,cur);
  return true;
}

BasicObjectPtr StackFrame::ReturnValue(){
  assert(pimpl->local_stack.size() > 0);
  return pimpl->local_stack.back();
}

void StackFrame::PushResult(const BasicObjectPtr& obj){
  pimpl->local_stack.push_back(obj.get());
}

void MarkVector(object_vector* v){
  for(object_vector::iterator it = v->begin();
      it != v->end();
      it++)
    (*it)->Mark();
}

BasicObjectPtr StackFrame::Binding(){
  return pimpl->binding;
}

void StackFrame::SetBinding(const BasicObjectPtr& obj){
  pimpl->binding = obj.get();
}

void StackFrame::Mark(){
  MarkVector(&pimpl->expressions);
  MarkVector(&pimpl->operations);
  MarkVector(&pimpl->local_stack);
}

// TODO: call StackFrame()
StackFrame::StackFrame(const StackFrame& obj):pimpl(new Impl()){
  pimpl->expressions = obj.pimpl->expressions;
  pimpl->tree_it = obj.pimpl->tree_it;
  pimpl->operations = obj.pimpl->operations;
  pimpl->it = obj.pimpl->it;
  pimpl->binding = obj.pimpl->binding;
  pimpl->local_stack = obj.pimpl->local_stack;
  pimpl->up_frame = obj.pimpl->up_frame;
}

StackFrame &StackFrame::operator=(const StackFrame& obj){
  if(this == &obj)
    return *this;
  pimpl->expressions = obj.pimpl->expressions;
  pimpl->tree_it = obj.pimpl->tree_it;
  pimpl->operations = obj.pimpl->operations;
  pimpl->it = obj.pimpl->it;
  pimpl->binding = obj.pimpl->binding;
  pimpl->local_stack = obj.pimpl->local_stack;
  pimpl->up_frame = obj.pimpl->up_frame;
  return *this;
}

