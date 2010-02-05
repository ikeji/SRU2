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
             const std::vector<std::string>& varg,
             const std::string& retval,
             const ptr_vector& expressions,
             const BasicObjectPtr& binding);
  virtual void Call(const ptr_vector& arg) = 0;
  virtual std::string Inspect();
  static void InitializeClassObject(const BasicObjectPtr& proc);
 protected:
  static void Initialize(const BasicObjectPtr& ptr);
};

} // namespace sru

#endif  // PROC_H_
