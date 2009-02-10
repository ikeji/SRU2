// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
//
// Interpreter Class.
// You can eval sru program and syntax tree.
//

#ifndef INTERPRETER_H_
#define INTERPRETER_H_

namespace sru {

class BasicObjectPtr;

class Interpreter{
 public:
  BasicObjectPtr Eval(BasicObjectPtr obj);
  static Interpreter* Instance(){
    static Interpreter inst;
    return &inst;
  }
  BasicObjectPtr CurrentStackFrame();
 private:
  Interpreter();
  ~Interpreter();
  
  struct Impl;
  Impl *pimpl;
  
  Interpreter(const Interpreter& obj);
  Interpreter* &operator=(const Interpreter& obj);
};

} // namespace sru

#endif  // INTERPRETER_H_
