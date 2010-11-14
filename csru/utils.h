// Programing Language SRU
// Copyright(C) 2005-2010 IKeJI
//

#ifndef UTILS_H_
#define UTILS_H_

#include <string>
#include "library.h"

namespace sru {
class BasicObject;
class BasicObjectPtr;

void PrintErrorPosition(const std::string& src, size_t pos);

bool IsNil(const BasicObjectPtr& ptr);
bool IsNil(const BasicObject* ptr);

inline BasicObjectPtr BooleanToObject(bool b){
  return b ? Library::Instance()->True():
             Library::Instance()->False();
}

}  // namespace sru

#endif // UTILS_H_
