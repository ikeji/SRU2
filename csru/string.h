// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 
// SRUString class is all class of all string in SRU.
//

#ifndef SRU_STRING_H_
#define SRU_STRING_H_

#include <string>
#include "basic_object.h"

namespace sru {

class SRUString : public Value{
 public:
  static BasicObjectPtr New(const std::string& val);
  static const std::string& GetValue(BasicObjectPtr obj);
  std::string Inspect();
  static const char* name(){ return "SRUString"; }
 private:
  SRUString(const std::string& val);
  ~SRUString();

  struct Impl;
  Impl* pimpl;

  SRUString(const SRUString& obj);
  SRUString* operator=(const SRUString& obj);
};

} // namespace sru

#endif  // SRU_STRING_H_


