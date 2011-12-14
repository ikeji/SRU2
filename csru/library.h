// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
//
// Library class.
// Library class have many default classes in SRU.
// You can get pointer to these class from Library.
//

#ifndef LIBRARY_H_
#define LIBRARY_H_

#include "basic_object.h"

namespace sru {

class ASTClass;

class Library {
 public:
  static Library* Instance();

  // Nil is zen
  BasicObjectPtr Nil() const;

  // Class of Binding system
  BasicObjectPtr Binding() const;

  // Default Class System
  BasicObjectPtr Class() const;
  BasicObjectPtr Object() const;
  BasicObjectPtr Proc() const;

  // Default Classes
  BasicObjectPtr Array() const;
  BasicObjectPtr Hash() const;
  BasicObjectPtr String() const;
  BasicObjectPtr Numeric() const;
  BasicObjectPtr Boolean() const;
  BasicObjectPtr True() const;
  BasicObjectPtr False() const;
  BasicObjectPtr Parser() const;
  BasicObjectPtr Math() const;
  BasicObjectPtr Sys() const;

  ASTClass *Ast() const;

  static void BindPrimitiveObjects(const BasicObjectPtr& obj);
 private:
  Library();
  ~Library();

  struct Impl;
  Impl *pimpl;

  Library(const Library& obj);
  Library &operator=(const Library& obj);
};

} // namespace sru

#endif  // LIBRARY_H_
