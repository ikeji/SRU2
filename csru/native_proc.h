// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
//
// NativeProc is a Proc.
// All native method a subclass of NativeProc.
//

#ifndef NATIVE_PROC_H_
#define NATIVE_PROC_H_

#include <string>
#include "proc.h"
#include "object_vector.h"

namespace sru {

class NativeProc: public Proc{
 public:
  void Call(const ptr_vector& arg);
  BasicObjectPtr New(){
    BasicObjectPtr result = BasicObject::New(this);
    Initialize(result);
    return result;
  }
  virtual BasicObjectPtr method_body(const ptr_vector& arg) = 0;
  virtual std::string Inspect();
};

#define DEFINE_SRU_PROC(name) \
  class METHOD_##name:public NativeProc { \
   private: \
    BasicObjectPtr method_body(const ptr_vector& arg); \
  }; \
  NativeProc* name = new METHOD_##name(); \
  BasicObjectPtr METHOD_##name::method_body(const ptr_vector& arg)

#define DECLARE_SRU_PROC(name) \
  extern NativeProc* name

#define CREATE_SRU_PROC(name) \
  (name->New())

} // namespace sru

#endif  // NATIVE_PROC_H_

