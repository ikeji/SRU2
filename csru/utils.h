// Programing Language SRU
// Copyright(C) 2005-2010 IKeJI
//

#ifndef UTILS_H_
#define UTILS_H_

#include <string>

namespace sru {
class BasicObject;
class BasicObjectPtr;

void PrintErrorPosition(const std::string& src, size_t pos);

bool IsNil(const BasicObjectPtr& ptr);
bool IsNil(const BasicObject* ptr);

}  // namespace sru

#endif // UTILS_H_
