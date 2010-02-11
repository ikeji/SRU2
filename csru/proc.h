// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
//
// Proc class is Class of all procedure in SRU.
//

#ifndef PROC_H_
#define PROC_H_

#include <vector>
#include <string>
#include "basic_object.h"
#include "object_vector.h"

namespace sru {

class Proc : public Value{
 public:
  static BasicObjectPtr New(
             const std::vector<std::string>& vargs,
             const std::string& retval,
             const ptr_vector& expressions,
             const BasicObjectPtr& binding);
  virtual void Call(const BasicObjectPtr& proc, const ptr_vector& args) = 0;
  virtual std::string Inspect();
  static void InitializeClassObject(const BasicObjectPtr& proc);
  static void Invoke(const BasicObjectPtr& proc, const ptr_vector& args);
  static void Initialize(const BasicObjectPtr& ptr);
};

} // namespace sru

#endif  // PROC_H_
