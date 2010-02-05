// Programing Language SRU
// Copyright(C) 2005-2010 IKeJI
//

#ifndef BOOLEAN_H_
#define BOOLEAN_H_

namespace sru {

class BasicObjectPtr;

void InitializeBooleanClassObject(const BasicObjectPtr& boolean);
void InitializeTrueObject(const BasicObjectPtr& tlue);
void InitializeFalseObject(const BasicObjectPtr& farse);

}  // namespace sru

#endif  // BOOLEAN_H_
