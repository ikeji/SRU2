// Programing Language SRU
// Copyright(C) 2005-2008 IKeJI
// 

#include "library.h"

#include "basic_object.h"
#include "numeric.h"
#include "parser.h"
#include "object.h"
#include "string.h"
#include "constants.h"

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
      Numeric(),
      Boolean(),
      True(),
      False(),
      Parser() {}
     
  BasicObjectPtr Nil;
  BasicObjectPtr Binding;
  BasicObjectPtr Class;
  BasicObjectPtr Object;
  BasicObjectPtr Proc;
  BasicObjectPtr Array;
  BasicObjectPtr Hash;
  BasicObjectPtr String;
  BasicObjectPtr Numeric;
  BasicObjectPtr Boolean;
  BasicObjectPtr True;
  BasicObjectPtr False;
  BasicObjectPtr Parser;
  
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
BasicObjectPtr Library::Numeric(){ return pimpl->Numeric; }
BasicObjectPtr Library::Boolean(){ return pimpl->Boolean; }
BasicObjectPtr Library::True(){ return pimpl->True; }
BasicObjectPtr Library::False(){ return pimpl->False; }

BasicObjectPtr Library::Parser(){ return pimpl->Parser; }

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
  Numeric = BasicObject::New();
  Boolean = BasicObject::New();
  True = BasicObject::New();
  False = BasicObject::New();
  Parser = BasicObject::New();

  // Initialize each Objects
  // TODO: Impliment Object Initialize.
  InitializeObjectClass(Object);

  // TODO: Move this to proc.cc
  Proc->Set(fNAME, SRUString::New("Proc"));
  // TODO: Move this to array.cc
  Array->Set(fNAME, SRUString::New("Array"));
  // TODO: Move this to hash.cc
  Hash->Set(fNAME, SRUString::New("Hash"));
  // TODO: Move this to string.cc
  String->Set(fNAME, SRUString::New("String"));
  // TODO: Move this to boolean.cc
  Boolean->Set(fNAME, SRUString::New("Boolean"));
  // TODO: Move this to boolean.cc
  True->Set(fNAME, SRUString::New("True"));
  // TODO: Move this to boolean.cc
  False->Set(fNAME, SRUString::New("False"));

  SRUNumeric::InitializeClassObject(Numeric);

  sru_parser::InitializeParserObject(Parser);
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
  frame->Set("Numeric",Instance()->Numeric());
  frame->Set("Boolean",Instance()->Boolean());
  frame->Set("true",Instance()->True());
  frame->Set("false",Instance()->False());
  frame->Set("sru_parser", Instance()->Parser());
}
