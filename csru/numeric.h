// Programing Language SRU
// Copyright(C) 2005-2009 IKeJI
//
// SRUNumeric
//

#ifndef SRU_NUMERIC_H_
#define SRU_NUMERIC_H_

#include <string>
#include "basic_object.h"

namespace sru {

class SRUNumeric : public Value{
 public:
  static void InitializeClassObject(const BasicObjectPtr& numeric);
  static BasicObjectPtr New(int i);
  static int GetValue(const BasicObjectPtr& obj);
  std::string Inspect();
  static const char* name(){ return "SRUNumeric"; }
  void Dispose();
 private:
  SRUNumeric(int i);
  ~SRUNumeric();

  struct Impl;
  Impl* pimpl;

  SRUNumeric(const SRUNumeric& obj);
  SRUNumeric* operator=(const SRUNumeric& obj);
};

} // namespace sru

#endif  // SRU_NUMERIC_H_
