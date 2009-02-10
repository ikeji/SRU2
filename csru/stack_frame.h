// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#ifndef STACK_FRAME_H_
#define STACK_FRAME_H_

#include "basic_object.h"
#include "object_vector.h"

namespace sru {

class StackFrame : public Value{
 public:
  static BasicObjectPtr New();

  StackFrame();
  ~StackFrame();

  void Setup(const ptr_vector& expressions);
  void SetUpStack(BasicObjectPtr obj);
  bool EndOfTrees();
  bool EvalNode();
  BasicObject* ReturnValue();

  void Mark();

  StackFrame(const StackFrame& obj);
  StackFrame &operator=(const StackFrame& obj);
 private:
  struct Impl;
  Impl *pimpl;
};

} // namespace sru

#endif  // STACK_FRAME_H_
