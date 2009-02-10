// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
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
  static BasicObjectPtr New(const std::vector<std::string>& varg,
             const std::string& retval,
             const ptr_vector& expressions);
  virtual void Call(const ptr_vector& arg) = 0;
};

} // namespace sru

#endif  // PROC_H_
