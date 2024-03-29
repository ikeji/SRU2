// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
//
// SRUString class is all class of all string in SRU.
//

#ifndef SRU_STRING_H_
#define SRU_STRING_H_

#include <string>
#include "basic_object.h"
#include "symbol.h"

namespace sru {

class SRUString : public Value{
 public:
  static BasicObjectPtr New(const symbol& val);
  static const symbol& GetValue(const BasicObjectPtr& obj);
  static const symbol& GetValue(const SRUString* obj);
  std::string Inspect();
  static const char* name(){ return "SRUString"; }
  static void InitializeStringClass(const BasicObjectPtr& str);

  static std::string EscapeString(const std::string& str);
  static std::string UnEscapeString(const std::string& str);
 private:
  SRUString(const symbol& val);
  ~SRUString() {};

  symbol value;

  SRUString(const SRUString& obj);
  SRUString* operator=(const SRUString& obj);

  void Dispose();
};

} // namespace sru

#endif  // SRU_STRING_H_
