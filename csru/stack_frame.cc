// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#include "stack_frame.h"

#include <vector>
#include "object_container.h"
#include "basic_object.h"
#include "sru_string.h"
#include "ast.h"
#include "proc.h"
#include "library.h"
#include "binding.h"
#include "constants.h"
#include "logging.h"
#include "utils.h"

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

  bool SetupTree(const BasicObjectPtr& ast);
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
  void VisitTo(const BasicObjectPtr& obj){
    Expression* exp = obj->GetData<Expression>();
    CHECK(exp) << "Trace non ast value";
    exp->Visit(this, obj);
  }
  void Accept(LetExpression* exp,const BasicObjectPtr& obj){
    LOG_TRACE << "TRACE-LET: " << exp->Inspect();
    if(!IsNil(exp->Env()))
      VisitTo(exp->Env());
    VisitTo(exp->RightValue());
    result->push_back(obj.get());
  }
  void Accept(RefExpression* exp,const BasicObjectPtr& obj){
    LOG_TRACE << "TRACE-REF: " << exp->Inspect();
    if(!IsNil(exp->Env()))
      VisitTo(exp->Env());
    result->push_back(obj.get());
  }
  void Accept(CallExpression* exp,const BasicObjectPtr& obj){
    LOG_TRACE << "TRACE-CALL: " << exp->Inspect();
    LOG_TRACE << "TRACE-CALL(PROC)";
    VisitTo(exp->Proc());
    LOG_TRACE << "TRACE-CALL(ARGS)";
    for(object_vector::const_iterator it = exp->Arg()->begin();
        it != exp->Arg()->end();
        it++)
      VisitTo(*it);
    result->push_back(obj.get());
  }
  void Accept(ProcExpression* exp,const BasicObjectPtr& obj){
    LOG_TRACE << "TRACE-PROC: " << exp->Inspect();
    result->push_back(obj.get());
  }
  void Accept(StringExpression* exp,const BasicObjectPtr& obj){
    LOG_TRACE << "TRACE-STRING: " << exp->Inspect();
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
    const BasicObjectPtr r = local_stack->back();
    local_stack->pop_back();
    return r;
  }
 private:
  void Push(const BasicObjectPtr& obj){
    assert(obj.get());
    local_stack->push_back(obj.get());
  }
 public:
  void Accept(LetExpression* exp,const BasicObjectPtr& obj){
    const BasicObjectPtr& rightValue = Pop();
    BasicObjectPtr env;
    if(!IsNil(exp->Env())){
      env = Pop();
    }else{
      env = binding;
    }

    // find slot
    BasicObjectPtr e = env;
    const symbol& name = SRUString::GetValue(exp->Name());
    while(!(e->HasSlot(name))){
      if(! e->HasSlot(sym::_parent())) break;
      e = e->Get(sym::_parent());
    }
    if(e->HasSlot(name)){
      // if found exist slot, use the slot.
      e->Set(name,rightValue);
    }else{
      // if not found exist slot, use current frame's enviroment.
      env->Set(name,rightValue);
    }
    LOG << "EVAL-LET: " << name.to_str() << " = " << rightValue->Inspect();
    LOG << "CURRENT-SCOPE: " << binding->Inspect();
    Push(rightValue);
  }
  void Accept(RefExpression* exp,const BasicObjectPtr& obj){
    BasicObjectPtr env;
    if(!IsNil(exp->Env())){
      env = Pop();
    }else{
      env = binding;
    }
    // find slot
    const symbol& name = SRUString::GetValue(exp->Name());
    if(env->HasSlot(name)){
      LOG << "EVAL-REF: " << env->Get(name)->Inspect();
      Push(env->Get(name));
      return;
    }
    if(env->HasSlot(sym::__findSlot())){
      LOG << "EVAL-REF-FIND_SLOT";
      ptr_vector args;
      args.push_back(env);
      args.push_back(exp->Name());
      const BasicObjectPtr find_slot_proc = env->Get(sym::__findSlot());
      Proc::Invoke(obj, find_slot_proc, args);
      return;
    }
    // if not found exist slot
    LOG << "EVAL-REF: nil ";
    Push(Library::Instance()->Nil());
  }
  void Accept(CallExpression* exp,const BasicObjectPtr& obj){
    // Get arg value from stack, but it on reverse order.
    ptr_vector reverse_args;
    for(object_vector::const_iterator it = exp->Arg()->begin();
        it != exp->Arg()->end();
        it++)
      reverse_args.push_back(Pop());

    // Reorder.
    ptr_vector args;
    for(ptr_vector::reverse_iterator it = reverse_args.rbegin();
        it != reverse_args.rend();
        it++)
      args.push_back(*it);

    const BasicObjectPtr proc = Pop();
    LOG << "EVAL-CALL: { ";
    Proc::Invoke(obj, proc, args);
    LOG << "EVAL-CALL: } ";
    // Caller must push return value.
  }
  void Accept(ProcExpression* exp,const BasicObjectPtr& obj){
    LOG << "EVAL-PROC: {} ";
    Push(Proc::New(
             *(exp->Varg()),exp->RetVal(),
             Conv(*(exp->Expressions())),binding));
  }
  void Accept(StringExpression* exp,const BasicObjectPtr& obj){
    LOG << "EVAL-STRING:" << exp->String().to_str();
    Push(SRUString::New(exp->String()));
  }
  object_vector* local_stack;
  BasicObjectPtr binding;
  bool noerror;
 private:
  EvalVisitor(const EvalVisitor& obj);
  EvalVisitor* operator=(const EvalVisitor& obj);
};

bool StackFrame::Impl::SetupTree(const BasicObjectPtr& ast){
  LOG << "SetupTree: " << ast->GetData<Expression>()->Inspect();
  local_stack.clear();
  operations.clear();
  TraceVisitor tracer(&operations);
  tracer.VisitTo(ast);
  it = 0;
  return tracer.noerror;
}

void StackFrame::Setup(const ptr_vector& asts){
  IF_DEBUG_TRACE{
    LOGOBJ(log);
    log.ostream() << "Setup: ";
    for(ptr_vector::const_iterator it = asts.begin();
        it != asts.end();
        it++){
      log.ostream() << (*it)->GetData<Expression>()->Inspect();
    }
  }
  pimpl->expressions = Conv(asts);
  pimpl->tree_it = 0;
  pimpl->operations.clear();
  pimpl->it = 0;

  // Return nil when expressions is empty.
  PushResult(Library::Instance()->Nil());
}

void StackFrame::SetUpperStack(const BasicObjectPtr& obj){
  LOG_TRACE << "SetUpperStack";
  pimpl->upper_frame = obj.get();
}

BasicObjectPtr StackFrame::GetUpperStack(){
  return(pimpl->upper_frame);
}

bool StackFrame::EndOfTrees(){
  LOG_TRACE << "EndOfTrees?:"
            << " root?:" <<
                (pimpl->upper_frame == NULL)
            << " lastline?:" <<
                (pimpl->expressions.size() == pimpl->tree_it)
            << " lastop?:" << 
                (pimpl->operations.size() == pimpl->it);
  return pimpl->upper_frame == NULL &&
         pimpl->expressions.size() == pimpl->tree_it &&
         pimpl->operations.size() == pimpl->it;
}

bool StackFrame::EvalNode(){
  LOG_TRACE << "EvalNode";
  assert(!EndOfTrees());
  if(pimpl->operations.size() == pimpl->it){
    LOG_TRACE << "LastOperation";
    if(pimpl->expressions.size() == pimpl->tree_it){
      LOG_TRACE << "LastExpression";
      const BasicObjectPtr rv = ReturnValue();
      StackFrame* st = pimpl->upper_frame->GetData<StackFrame>();
      if(st == NULL)
        return false;
      LOG_TRACE << "Step Out:" << pimpl->upper_frame->Inspect();
      LOG << "Reutn: " << rv->Inspect();
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
  const BasicObjectPtr cur = pimpl->operations[pimpl->it];
  pimpl->it++;
  LOG << "Eval: " << cur->GetData<Expression>()->Inspect();
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
  if(pimpl->binding) pimpl->binding->Mark();
  if(pimpl->upper_frame) pimpl->upper_frame->Mark();
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

void StackFrame::Dispose(){
  delete this;
}
