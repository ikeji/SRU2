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

class BasicObjectPtr;

class NativeProc: public Proc{
 public:
  void Call(const BasicObjectPtr& ptr, const ptr_vector& arg);
  BasicObjectPtr New(){
    BasicObjectPtr result = BasicObject::New(this);
    Initialize(result);
    return result;
  }
  virtual BasicObjectPtr method_body(const BasicObjectPtr& proc,
                                     const ptr_vector& arg) = 0;
  virtual std::string Inspect();
};

// TODO: Change object hierarchy.
class NativeProcWithStackSmash: public NativeProc{
 public:
  void Call(const BasicObjectPtr& ptr, const ptr_vector& arg);
  // TODO: Remove this.
  BasicObjectPtr method_body(const BasicObjectPtr& proc,
                             const ptr_vector& arg){assert(false);return NULL;};
  virtual void method_body_smash(const BasicObjectPtr& proc,
                                 const ptr_vector& arg) = 0;
};

#define DEFINE_SRU_PROC(name) \
  class METHOD_##name:public NativeProc { \
   private: \
    BasicObjectPtr method_body(const BasicObjectPtr& proc, \
                               const ptr_vector& arg); \
  }; \
  NativeProc* name = new METHOD_##name(); \
  BasicObjectPtr METHOD_##name::method_body(const BasicObjectPtr& proc, \
                                            const ptr_vector& arg)

#define DEFINE_SRU_PROC_SMASH(name) \
  class METHOD_##name:public NativeProcWithStackSmash { \
   private: \
    void method_body_smash(const BasicObjectPtr& proc, \
                           const ptr_vector& arg); \
  }; \
  NativeProc* name = new METHOD_##name(); \
  void METHOD_##name::method_body_smash(const BasicObjectPtr& proc, \
                                        const ptr_vector& arg)

#define DECLARE_SRU_PROC(name) \
  extern NativeProc* name

#define CREATE_SRU_PROC(name) \
  (name->New())

} // namespace sru

#endif  // NATIVE_PROC_H_

