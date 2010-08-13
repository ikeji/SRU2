// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
//

#ifndef SRU_ARRAY_H_
#define SRU_ARRAY_H_

#include "basic_object.h"

namespace sru {

class Array : public Value {
 public:
  static void InitializeClass(const BasicObjectPtr& array);
  static BasicObjectPtr New();
  static const char* name(){ return "Array"; }
 private:
  Array();
  ~Array();
  struct Impl;
  Impl* pimpl;
  
  Array(const Array& obj);
  Array &operator=(const Array& obj);
  void Dispose();
};

} // namespace sru

#endif  // SRU_ARRAY_H_ 
