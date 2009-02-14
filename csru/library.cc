// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#include "library.h"

#include "basic_object.h"
#include "object.h"

using namespace sru;

struct Library::Impl {
  Impl():
      Nil(),
      Binding(),
      Class(),
      Object(),
      Proc(),
      Array(),
      Hash(),
      String(),
      Number(),
      Boolean(),
      True(),
      False() {}
     
  BasicObjectPtr Nil;
  BasicObjectPtr Binding;
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

BasicObjectPtr Library::Binding(){ return pimpl->Binding; }

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
  static bool initialized = false;
  if(!initialized){
    initialized = true;
    inst.pimpl->initialiseInteralClasses();
  }
  return &inst;
}

Library::Library():pimpl(new Impl()){
}
Library::~Library(){
  delete pimpl;
}

void Library::Impl::initialiseInteralClasses(){
  // Set new objects
  Nil = BasicObject::New();
  Binding = BasicObject::New();
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
  InitializeObjectClass(Object);
}

void Library::BindPrimitiveObjects(const BasicObjectPtr& frame){
  frame->Set("nil",Instance()->Nil());
  frame->Set("Binding",Instance()->Binding());
  frame->Set("Class",Instance()->Class());
  frame->Set("Object",Instance()->Object());
  frame->Set("Proc",Instance()->Proc());
  frame->Set("Array",Instance()->Array());
  frame->Set("Hash",Instance()->Hash());
  frame->Set("String",Instance()->String());
  frame->Set("Number",Instance()->Number());
  frame->Set("Boolean",Instance()->Boolean());
  frame->Set("true",Instance()->True());
  frame->Set("false",Instance()->False());
}
