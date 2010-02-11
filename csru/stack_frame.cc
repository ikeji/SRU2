// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#include "stack_frame.h"

#include <iostream>
#include <vector>
#include "object_vector.h"
#include "basic_object.h"
#include "string.h"
#include "ast.h"
#include "proc.h"
#include "library.h"
#include "binding.h"

using namespace std;
using namespace sru;

struct StackFrame::Impl {
  Impl(const BasicObjectPtr& binding):
    expressions(),
    tree_it(0),
    operations(),
    it(0),
    local_stack(),
    binding(binding.get()),
    upper_frame(NULL)
    {}

  object_vector expressions;
  unsigned int tree_it;

  object_vector operations;
  unsigned int it;

  object_vector local_stack;
 
  BasicObject* binding; 
  BasicObject* upper_frame;

  bool SetupTree(BasicObjectPtr ast);
 private:
  Impl(const Impl& obj);
  Impl* operator=(const Impl& obj);
};

BasicObjectPtr StackFrame::New(const BasicObjectPtr& binding){
  return BasicObject::New(new StackFrame(binding));
}

StackFrame::StackFrame(const BasicObjectPtr& binding):
    pimpl(new Impl(binding)){
}

StackFrame::~StackFrame(){
  delete pimpl;
}

class TraceVisitor : public Visitor{
 public:
  TraceVisitor(object_vector* res):
        result(res),noerror(true){}
  void VisitTo(BasicObjectPtr obj){
    obj->GetData<Expression>()->Visit(this, obj);
  }
  void Accept(LetExpression* exp,const BasicObjectPtr& obj){
#ifdef DEBUG
    cout << "TRACE-LET: " << exp->Inspect() << endl;
#endif
    if(exp->Env())
      VisitTo(exp->Env());
    VisitTo(exp->RightValue());
    result->push_back(obj.get());
  }
  void Accept(RefExpression* exp,const BasicObjectPtr& obj){
#ifdef DEBUG
    cout << "TRACE-REF: " << exp->Inspect() << endl;
#endif
    if(exp->Env())
      VisitTo(exp->Env());
    result->push_back(obj.get());
  }
  void Accept(CallExpression* exp,const BasicObjectPtr& obj){
#ifdef DEBUG
    cout << "TRACE-CALL: " << exp->Inspect() << endl;
    cout << "TRACE-CALL(PROC)" << endl;
#endif
    VisitTo(exp->Proc());
#ifdef DEBUG
    cout << "TRACE-CALL(ARGS)" << endl;
#endif
    for(object_vector::const_iterator it = exp->Arg().begin();
        it != exp->Arg().end();
        it++)
      VisitTo(*it);
    result->push_back(obj.get());
  }
  void Accept(ProcExpression* exp,const BasicObjectPtr& obj){
#ifdef DEBUG
    cout << "TRACE-PROC: " << exp->Inspect() << endl;
#endif
    result->push_back(obj.get());
  }
  void Accept(StringExpression* exp,const BasicObjectPtr& obj){
#ifdef DEBUG
    cout << "TRACE-STRING: " << exp->Inspect() << endl;
#endif
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

    // find slot
    BasicObjectPtr e = env;
    while(!(e->HasSlot(exp->Name()))){
      if(! e->HasSlot(fPARENT_SCOPE)) break;
      e = e->Get(fPARENT_SCOPE);
    }
    if(e->HasSlot(exp->Name())){
      // if found exist slot, use the slot.
      e->Set(exp->Name(),rightValue);
    }else{
      // if not found exist slot, use current frame's enviroment.
      env->Set(exp->Name(),rightValue);
    }
#ifdef DEBUG
    cout << "EVAL-LET: " << exp->Name() << " = " << rightValue->Inspect() << endl;
    cout << "CURRENT-SCOPE: " << binding->Inspect() << endl;
#endif
    Push(rightValue);
  }
  void Accept(RefExpression* exp,const BasicObjectPtr& obj){
    BasicObjectPtr env;
    if(exp->Env()){
      env = Pop();
    }else{
      env = binding;
    }
    // find slot
    BasicObjectPtr e = env;
    while(!(e->HasSlot(exp->Name()))){
      if(! e->HasSlot(fPARENT_SCOPE)) break;
      e = e->Get(fPARENT_SCOPE);
    }
    if(e->HasSlot(exp->Name())){
#ifdef DEBUG
      cout << "EVAL-REF: " << e->Get(exp->Name())->Inspect() << endl;
#endif
      // if found exist slot
      Push(e->Get(exp->Name()));
    }else{
#ifdef DEBUG
      cout << "EVAL-REF: nil " << endl;
#endif
      // if not found exist slot
      Push(Library::Instance()->Nil());
    }
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
#ifdef DEBUG
    cout << "EVAL-CALL: { " << endl;
#endif
    Proc::Invoke(proc,args);
#ifdef DEBUG
    cout << "EVAL-CALL: } " << endl;
#endif
    // Caller must push return value.
  }
  void Accept(ProcExpression* exp,const BasicObjectPtr& obj){
#ifdef DEBUG
    cout << "EVAL-PROC: {} " << endl;
#endif
    Push(Proc::New(
             exp->Varg(),exp->RetVal(),
             Conv(exp->Expressions()),binding));
  }
  void Accept(StringExpression* exp,const BasicObjectPtr& obj){
#ifdef DEBUG
    cout << "EVAL-STRING:" << exp->String() << endl;
#endif
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
  cout << "SetupTree: " << ast->GetData<Expression>()->Inspect() << endl;
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
    cout << (*it)->GetData<Expression>()->Inspect();
  }
  cout << endl;
#endif
  pimpl->expressions = Conv(asts);
  pimpl->tree_it = 0;
  pimpl->operations.clear();
  pimpl->it = 0;
}

void StackFrame::SetUpperStack(BasicObjectPtr obj){
#ifdef DEBUG
  cout << "SetUpperStack" << endl;
#endif
  pimpl->upper_frame = obj.get();
}

BasicObjectPtr StackFrame::GetUpperStack(){
  return(pimpl->upper_frame);
}

bool StackFrame::EndOfTrees(){
#ifdef DEBUG
  cout << "EndOfTrees?:"
       << " root?:" <<
           (pimpl->upper_frame == NULL)
       << " lastline?:" <<
           (pimpl->expressions.size() == pimpl->tree_it)
       << " lastop?:" << 
           (pimpl->operations.size() == pimpl->it) << endl;
#endif
  return pimpl->upper_frame == NULL &&
         pimpl->expressions.size() == pimpl->tree_it &&
         pimpl->operations.size() == pimpl->it;
}

bool StackFrame::EvalNode(){
#ifdef DEBUG
  cout << "EvalNode" << endl;
#endif
  assert(!EndOfTrees());
  if(pimpl->operations.size() == pimpl->it){
#ifdef DEBUG
    cout << "LastOperation" << endl;
#endif
    if(pimpl->expressions.size() == pimpl->tree_it){
#ifdef DEBUG
      cout << "LastExpression" << endl;
#endif
      BasicObjectPtr rv = ReturnValue();
      StackFrame* st = pimpl->upper_frame->GetData<StackFrame>();
      if(st == NULL)
        return false;
#ifdef DEBUG
      cout << "Step Out:" << pimpl->upper_frame->Inspect() << endl;
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
#ifdef DEBUG
  cout << "Eval: ";
  cout << cur->GetData<Expression>()->Inspect();
  cout << endl;
#endif
  // Execute immidentry
  Expression* exp = cur->GetData<Expression>();
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
StackFrame::StackFrame(const StackFrame& obj):
      pimpl(new Impl(obj.pimpl->binding)){
  pimpl->expressions = obj.pimpl->expressions;
  pimpl->tree_it = obj.pimpl->tree_it;
  pimpl->operations = obj.pimpl->operations;
  pimpl->it = obj.pimpl->it;
  pimpl->local_stack = obj.pimpl->local_stack;
  pimpl->upper_frame = obj.pimpl->upper_frame;
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
  pimpl->upper_frame = obj.pimpl->upper_frame;
  return *this;
}

