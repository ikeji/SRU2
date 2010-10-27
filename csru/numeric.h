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
  static BasicObjectPtr NewInt(int i);
  static BasicObjectPtr NewDouble(double i);
  static int GetIntValue(const BasicObjectPtr& obj);
  static double GetDoubleValue(const BasicObjectPtr& obj);
  static bool TryGetIntValue(const BasicObjectPtr& obj, int* val);
  static bool TryGetDoubleValue(const BasicObjectPtr& obj, double* val);
  static bool IsReal(const BasicObjectPtr& obj);
  std::string Inspect();
  static const char* name(){ return "SRUNumeric"; }
  void Dispose();
 private:
  SRUNumeric(int i);
  SRUNumeric(double i);
  ~SRUNumeric();

  struct Impl;
  Impl* pimpl;

  SRUNumeric(const SRUNumeric& obj);
  SRUNumeric* operator=(const SRUNumeric& obj);
};

} // namespace sru

#endif  // SRU_NUMERIC_H_
