// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
//
// Interpreter Class.
// You can eval sru program and syntax tree.
//

#ifndef INTERPRETER_H_
#define INTERPRETER_H_

#include <string>
#include "object_container.h"

namespace sru {

class BasicObjectPtr;
class StackFrame;

class Interpreter{
 public:
  BasicObjectPtr Eval(const BasicObjectPtr& obj);
  BasicObjectPtr Eval(const std::string& str);
  static Interpreter* Instance();
  // TODO: find more good name.
  void DigIntoNewFrame(const ptr_vector& expressions,
                       const BasicObjectPtr& binding);
  // TODO: find more good name.
  BasicObjectPtr GetContinationToEscapeFromCurrentStack();
  StackFrame* CurrentStackFrame();
  StackFrame* RootStackFrame();
 private:
  Interpreter();
  ~Interpreter();
  void InitializeInterpreter();

  struct Impl;
  Impl *pimpl;

  Interpreter(const Interpreter& obj);
  Interpreter* &operator=(const Interpreter& obj);
};

} // namespace sru

#endif  // INTERPRETER_H_
