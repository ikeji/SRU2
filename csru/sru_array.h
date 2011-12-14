// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
//

#ifndef SRU_ARRAY_H_
#define SRU_ARRAY_H_

#include "basic_object.h"
#include "object_container.h"
#include <string>

namespace sru {

class Array : public Value {
 public:
  static void InitializeClass(const BasicObjectPtr& array);
  static BasicObjectPtr New();
  static BasicObjectPtr New(const ptr_vector& array);
  static const char* name(){ return "Array"; }
  object_vector* GetValue(){ return &value; }
  virtual std::string Inspect();

  void Dispose();
  void Mark();
 private:
  Array();
  ~Array();
  object_vector value;
  
  Array(const Array& obj);
  Array &operator=(const Array& obj);
};

} // namespace sru

#endif  // SRU_ARRAY_H_
