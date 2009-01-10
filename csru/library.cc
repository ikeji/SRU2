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

BasicObject* Library::Nil(){ return pimpl->Nil.get(); }

BasicObject* Library::Frame(){ return pimpl->Frame.get(); }

BasicObject* Library::Class(){ return pimpl->Class.get(); }
BasicObject* Library::Object(){ return pimpl->Object.get(); }
BasicObject* Library::Proc(){ return pimpl->Proc.get(); }

BasicObject* Library::Array(){ return pimpl->Array.get(); }
BasicObject* Library::Hash(){ return pimpl->Hash.get(); }
BasicObject* Library::String(){ return pimpl->String.get(); }
BasicObject* Library::Number(){ return pimpl->Number.get(); }
BasicObject* Library::Boolean(){ return pimpl->Boolean.get(); }
BasicObject* Library::True(){ return pimpl->True.get(); }
BasicObject* Library::False(){ return pimpl->False.get(); }

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
  Nil.reset(BasicObject::New());
  Frame.reset(BasicObject::New());
  Class.reset(BasicObject::New());
  Object.reset(BasicObject::New());
  Proc.reset(BasicObject::New());
  Array.reset(BasicObject::New());
  Hash.reset(BasicObject::New());
  String.reset(BasicObject::New());
  Number.reset(BasicObject::New());
  Boolean.reset(BasicObject::New());
  True.reset(BasicObject::New());
  False.reset(BasicObject::New());

  // Initialize each Objects
  // TODO: Impliment Object Initialize.
}
