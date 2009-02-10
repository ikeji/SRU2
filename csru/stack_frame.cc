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

using namespace std;
using namespace sru;

struct StackFrame::Impl {
  Impl():
    expressions(),
    tree_it(),
    operations(),
    it(),
    local_stack(),
    up_frame(NULL) {}

  object_vector expressions;
  object_vector::iterator tree_it;

  object_vector operations;
  object_vector::iterator it;

  object_vector local_stack;
  
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
    VisitTo(exp->Env());
    VisitTo(exp->RightValue());
    result->push_back(obj.get());
  }
  void Accept(RefExpression* exp,const BasicObjectPtr& obj){
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
  EvalVisitor(object_vector* st): local_stack(st),noerror(true){}
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
    BasicObjectPtr rightValue = Pop();
    BasicObjectPtr env = Pop();
    env->Set(exp->Name(),rightValue);
    Push(rightValue);
  }
  void Accept(RefExpression* exp,const BasicObjectPtr& obj){
    BasicObjectPtr env = Pop();
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
    Push(Proc::New(exp->Varg(),exp->RetVal(),Conv(exp->Expressions())));
  }
  void Accept(StringExpression* exp,const BasicObjectPtr& obj){
    Push(SRUString::New(exp->String()));
  }
  object_vector* local_stack;
  bool noerror;
 private:
  EvalVisitor(const EvalVisitor& obj);
  EvalVisitor* operator=(const EvalVisitor& obj);
};

bool StackFrame::Impl::SetupTree(BasicObjectPtr ast){
  local_stack.clear();
  operations.clear();
  TraceVisitor tracer(&operations);
  tracer.VisitTo(ast);
  it = operations.begin();
  return tracer.noerror;
}

void StackFrame::Setup(const ptr_vector& asts){
  pimpl->expressions = Conv(asts);
  pimpl->tree_it = pimpl->expressions.begin();
  pimpl->operations.clear();
  pimpl->it = pimpl->operations.begin();
}

void StackFrame::SetUpStack(BasicObjectPtr obj){
  pimpl->up_frame = obj.get();
}

bool StackFrame::EndOfTrees(){
  return pimpl->up_frame == NULL &&
         pimpl->expressions.end() == pimpl->tree_it &&
         pimpl->operations.end() == pimpl->it;
}

bool StackFrame::EvalNode(){
  assert(!EndOfTrees());
  if(pimpl->operations.end() == pimpl->it){
    if(pimpl->expressions.end() == pimpl->tree_it){
      BasicObjectPtr rv = ReturnValue();
      StackFrame* st = dynamic_cast<StackFrame*>(pimpl->up_frame->Data());
      if(st == NULL)
        return false;
      *this = *st;
      pimpl->local_stack.push_back(rv.get());
      return true;
    }
    bool ret = pimpl->SetupTree(*pimpl->tree_it);
    pimpl->tree_it++;
    return ret;
  }
  EvalVisitor visit(&(pimpl->local_stack));
  // We need step forward before exec each code.
  BasicObjectPtr cur = *pimpl->it;
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
  pimpl->local_stack = obj.pimpl->local_stack;
  pimpl->up_frame = obj.pimpl->up_frame;
  return *this;
}
