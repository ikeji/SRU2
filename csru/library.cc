// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#include "library.h"
#include "basic_object.h"

using namespace sru;

struct Library::Impl {
  BasicObjectPtr Nil;
  BasicObjectPtr Frame;
  BasicObjectPtr Class;
  BasicObjectPtr Object;
  BasicObjectPtr Proc;
  BasicObjectPtr Array;
  BasicObjectPtr Hash;
  BasicObjectPtr String;
  BasicObjectPtr Number;
  BasicObjectPtr Boolean;
  BasicObjectPtr True;
  BasicObjectPtr False;
  
  void initialiseInteralClasses();
};

BasicObjectPtr Library::Nil(){ return pimpl->Nil; }

BasicObjectPtr Library::Frame(){ return pimpl->Frame; }

BasicObjectPtr Library::Class(){ return pimpl->Class; }
BasicObjectPtr Library::Object(){ return pimpl->Object; }
BasicObjectPtr Library::Proc(){ return pimpl->Proc; }

BasicObjectPtr Library::Array(){ return pimpl->Array; }
BasicObjectPtr Library::Hash(){ return pimpl->Hash; }
BasicObjectPtr Library::String(){ return pimpl->String; }
BasicObjectPtr Library::Number(){ return pimpl->Number; }
BasicObjectPtr Library::Boolean(){ return pimpl->Boolean; }
BasicObjectPtr Library::True(){ return pimpl->True; }
BasicObjectPtr Library::False(){ return pimpl->False; }

Library* Library::Instance(){
  static Library inst;
  return &inst;
}

Library::Library(){
  pimpl = new Impl();
  pimpl->initialiseInteralClasses();
}
Library::~Library(){
  delete pimpl;
}

void Library::Impl::initialiseInteralClasses(){
  // Set new objects
  Nil = BasicObject::New();
  Frame = BasicObject::New();
  Class = BasicObject::New();
  Object = BasicObject::New();
  Proc = BasicObject::New();
  Array = BasicObject::New();
  Hash = BasicObject::New();
  String = BasicObject::New();
  Number = BasicObject::New();
  Boolean = BasicObject::New();
  True = BasicObject::New();
  False = BasicObject::New();

  // Initialize each Objects
  // TODO: Impliment Object Initialize.
}
