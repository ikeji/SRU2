// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
//
// Utility type object_vector and ptr_vector.
// We can convert each type by Conv function.
//

#ifndef OBJECT_VECTOR_H_
#define OBJECT_VECTOR_H_

#include <vector>

namespace sru {

class BasicObject; 
class BasicObjectPtr;

typedef std::vector<BasicObject*> object_vector;
typedef std::vector<BasicObjectPtr> ptr_vector;

ptr_vector Conv(const object_vector& src);
object_vector Conv(const ptr_vector& src);

void MarkVector(object_vector* vec);

} // namespace sru

#endif  // OBJECT_VECTOR_H_
